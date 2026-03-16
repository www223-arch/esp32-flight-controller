Framework 分层与使用指南
=========================

概述
----
本组件提供飞控项目的分层框架：硬件抽象（`hal/`）、平台驱动（`drivers/`）、上层 C++ 管理/算法（`cpp/`），以及工具/中间件（`utils/`, `middleware/`, `control/`）。核心目标是高内聚、低耦合，便于替换硬件与单元测试。

模块概览
---------
- `hal/`：硬件抽象接口（纯 C 头），定义设备数据结构与最小 API（例如 `hal/imu.h`、`hal/imu_base.h`、`drivers/rc/rc.h`、`drivers/motor/motor.h`）。
- `drivers/`：设备平台实现（C），并在工厂内注册自身创建函数（legacy C factory）或通过 C++ 注册表被适配。
- `cpp/`：上层 C++ 管理器、工厂注册表、适配器、PubSub（实例化队列版）、控制算法示例（PID、Mixer）等。
- `pubsub/`：跨语言消息总线桥接（C++ 实例 + C 出口），用于任务间的事件/数据分发。
- `utils/`：日志后端、RTOS 辅助、后端实现（VOFA/UDP/File）等。

关键组件详述
--------------

**PubSub（C++ 实例化队列分发）**
- 现在为可实例化的 C++ 类（`cpp/PubSub.*`），内部使用 FreeRTOS 队列与分发任务，发布为异步入队，订阅回调在 dispatcher 任务中被调用。
- 提供 C 桥接函数（`pubsub_register_for_c` / `pubsub_unregister_for_c` / `pubsub_c_publish`），方便旧的 C 驱动把数据发布到某个 PubSub 实例（通常由对应 Manager 持有）。
- 使用场景：ImuManager 持有一个 `PubSub` 成员，驱动可以直接调用 C 桥 `pubsub_c_publish("imu/raw", &s, sizeof(s))`（在驱动初始化后先注册对应实例）。

**IMU 子系统（工厂类 + 产品类 + 注册表）**
- 设计：每种 IMU 有自己的工厂（负责创建/销毁产品），产品实现 `ImuBase` 抽象（C++）并封装具体行为。
- 实现细节：
  - `ImuBase`：C++ 抽象类（`open/read/close`）。
  - `ImuAdapter`：把现有 C `imu_dev_t` 包装为 `ImuBase`，方便渐进迁移复用 C 驱动实现。
  - `ImuFactoryRegistry`：运行时注册表，按名称（"bmi088"、"mpu6050"）创建对应 `std::unique_ptr<ImuBase>`。
  - `ImuManager`：持有 `std::unique_ptr<ImuBase>` 与一个 `PubSub` 实例（非单例），负责调用 `read()` 并通过 `m_pubsub.publish()` 派发数据；同时在 init 中调用 `pubsub_register_for_c(&m_pubsub)`，使 legacy C 驱动也能发布到该实例。
- 使用示例（C++）：
  - 初始化并启动：
    ```cpp
    auto &mgr = ImuManager::instance();
    mgr.init("bmi088");
    mgr.poll_once();
    int sub = mgr.subscribe([](const imu_sample_t &s){ /* 处理 */ });
    mgr.unsubscribe(sub);
    mgr.deinit();
    ```

**RC 与 Motor 子系统**
- RC：引入 `RCBase`（C++ 抽象）与 `RCAdapter`（封装 C `rc_dev_t`），并提供 `RCFactory::create(name)` 返回 `std::unique_ptr<RCBase>`。`RCManager` 使用 C++ 工厂与适配器。
- Motor：类似思路，`MotorFactory`（C++ 注册表） + `motor_dev_t` 兼容层。当前保留 C API wrapper 以兼容旧调用。

遗留 C API Wrapper 的作用
-------------------------
- 目的：向后兼容现有代码路径（`main/` 或第三方模块）无需一次性全部迁移到 C++。
- 策略：
  - 开发新模块或重构上层时优先使用 C++ 工厂/类接口（高内聚、RAII、智能指针）。
  - 逐步把调用点迁移到 C++，当所有使用处迁移完成后再删除 C wrapper。

如何在现有框架下新增设备/模块（步骤）
---------------------------------
1. 设计并实现纯 C 的底层驱动（如果需要兼容现有 C 驱动），放入 `drivers/<dev>/`，实现 `hal` 头中约定的 API。
2. 在 C++ 层实现一个 `*Factory` 或注册函数，把该驱动包装为 C++ 产品（或直接提供 C++ 实现）：
   - 推荐方式（运行时注册表）：在 `cpp/ImuFactoryRegistry.cpp` 中或单独工厂文件里注册一个 lambda：
     ```cpp
     ImuFactoryRegistry::instance().register_factory("myimu", [](){ return std::unique_ptr<ImuBase>(new MyImuProduct(...)); });
     ```
3. 在 `ImuManager::init("myimu")` 中即可按名称创建并使用。
4. 如需驱动直接发布数据至 Manager（C 驱动场景），在 Manager init 后调用 `pubsub_register_for_c(&m_pubsub)`，驱动用 `pubsub_c_publish(topic, data, len)` 发布。

如何调用（示例）
------------------
- C++（推荐，上层逻辑）：
  ```cpp
  ImuManager::instance().init("bmi088");
  ImuManager::instance().subscribe([](const imu_sample_t &s){ /* 处理 */ });
  ImuManager::instance().poll_once();
  ```

- C（向后兼容，驱动内部）：
  ```c
  // 驱动在初始化时无需关心 PubSub 实例细节；上层会在 Manager init 时注册 bridge
  extern void pubsub_c_publish(const char* topic, const void* data, unsigned short len);
  // 当有样本时：
  pubsub_c_publish("imu/raw", &sample, sizeof(sample));
  ```

构建与测试备注
----------------
- C++ 代码需要在 `CMakeLists.txt` 中把 `cpp/` 目录列入组件源文件，ESP-IDF 会自动处理混合编译。
- 推荐先在 Host 上用模拟 `imu_dev_t` 做单元测试，再在目标板上跑集成测试。

迁移与清理建议
----------------
- 先把新实现（C++ 工厂/注册表/适配器）放入 `cpp/` 并在上层使用，保留 C wrapper 一段时间用于兼容。
- 在确认所有调用点迁移完成后，删除对应的 `*_factory.c` / C-only API，并移除重复定义与过时头文件。

扩展与自定义
----------------
- 你可以为每个子系统选择不同工厂实现：模板工厂、注册表、抽象工厂或策略工厂，但要保证：
  - 工厂职责单一（只负责创建/销毁），
  - 产品对象封装具体行为并实现统一抽象（如 `ImuBase`），
  - 上层只依赖抽象，不依赖工厂实现细节。

需要我代为完成的操作（选项）
--------------------------------
- 为 `drivers/*` 中的每个遗留 C 工厂添加 `deprecated` 注释与迁移说明；或
- 运行一次完整的 ESP?IDF 构建并返回编译/链接错误（需要你激活 ESP?IDF 环境或提供安装路径）；或
- 把 README 中的示例代码逐步补成可执行示例（`main/` 的演示任务链）。

---

如需我现在把 README 中某节进一步展开成代码示例或把迁移脚本写好，请告诉我你优先的目标，我马上开始实现。
