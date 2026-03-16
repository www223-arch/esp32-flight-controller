# Framework 实现步骤与注意事项

本文档面向维护者与贡献者，列出将 Framework 从混合 C/C++ 驱动到按层次分离（应用层 C++，驱动层 C）的具体实现步骤与关键注意事项，便于逐步迁移、编译验证与排查问题。

## 前置条件
- 在开发机上已安装并能激活 ESP-IDF 环境（Windows 下通常通过 `export.bat` / `install.bat` 启动）。
- 熟悉 FreeRTOS 任务、队列与中断上下文限制。
- 熟悉组件化 CMake（ESP-IDF 的 component model）和混合语言编译（C/C++）。

## 高层目标
- 应用层尽量使用现代 C++（RAII、unique_ptr、std::array/ vector（只在主机测试时谨慎使用）），不在中断上下文使用堆分配。
- 底层驱动保持 C 接口和数据结构，提供最小且稳定的 ABI。
- 在 C/C++ 边界使用适配器（Adapter）与显式所有权转换（谁负责 free）。
- PubSub 使用 C++ 实例（非全局单例），并提供 C bridge 以便旧驱动发布数据。
- 工厂模式使用 C++ Registry（字符串或 enum→creator），便于运行时切换 IMU/电调实现。

## 逐步实现步骤

1. 环境与构建准备
   - 在 Windows PowerShell 中激活 ESP-IDF：运行项目根目录下的 `export.bat`（或用户本地的 idf 环境脚本）。
   - 在启用环境的终端运行 `idf.py fullclean`（可选），然后 `idf.py build` 验证当前编译基线。

2. 确定边界契约（Contract）
   - 为每个 C 驱动定义一个稳定的头文件（例如 `hal/imu.h`、`drivers/rc.h`、`drivers/motor.h`），说明初始化、读写、回调签名与释放函数。
   - 所有 C 接口使用 `extern "C"` 对 C++ 可见性友好。

3. 适配器（Adapter）与所有权
   - 对每个 C 设备创建一个 C++ 适配器类（例如 `ImuAdapter`）：持有指向 C 设备结构体的裸指针或引用，但不直接管理硬件资源的生命周期，除非明确包装初始化/释放。
   - 约定：初始化由 C 驱动（或工厂 shim）负责；C++ 适配器只负责在析构时解除订阅并释放轻量堆对象（若有）。

4. PubSub：实例化队列 + C bridge
   - 在关键 manager（如 `ImuManager`）中持有一个 `PubSub` 实例（FreeRTOS 队列 + 分发任务）。
   - 提供 C bridge 注册函数 `pubsub_register_dispatcher(void (*c_publish_cb)(...))`：底层 C 驱动在中断或 polling 时调用该 bridge，将数据移交给 C++ dispatcher（若在中断中调用，仅放入中断安全的环形缓冲或从 ISR 使用 `xQueueSendFromISR`）。
   - 明确订阅句柄类型与释放责任：`subscribe()` 返回一个句柄（struct/指针），`unsubscribe(handle)` 必须释放所有与之关联的 C++ 回调包装对象。

5. 工厂与注册表
   - 使用 C++ 注册表模式（map<string, CreatorFn> 或 enum→function 指针）。
   - 每个驱动实现（例如 `bmi088`、`mpu6050`）在自身的 component init 阶段调用注册函数注册到工厂。
   - 为兼容旧 C 创建 shim creator：C creator 调用新 C++ registry 或反之，保持迁移链路。

6. CMake 与编译器选项
   - 在 component 的 `CMakeLists.txt` 中区分 C 和 C++ 源：使用 `set_property(SOURCE ... PROPERTY LANGUAGE CXX)` 或将 `.cpp` 放入 `COMPONENT_SRCS`。
   - 禁用异常（ESP-IDF 默认可能禁用），请检查 `COMPONENT_CPPFLAGS` 是否含 `-fno-exceptions` 或 `-fno-rtti`；若使用 C++ 特性需确认配置一致。
   - 链接顺序：若 C++ 符号在静态对象初始化时依赖，确保 component registration 在运行时可用（使用显式 init 函数或 constructor attribute 谨慎）。

7. 多线程与中断注意事项
   - 驱动的回调若在 ISR 调用，回调体内只能做最小工作：写入 lock-free buffer 或使用 `xQueueSendFromISR`。
   - 不要在 ISR 中分配堆；若需要临时数据，使用 preallocated ring buffer 或本地静态池。
   - PubSub 分发任务应设定合适优先级，避免阻塞高优先级实时任务。

8. 资源与内存安全
   - 明确哪些对象在堆上分配（new/malloc），谁负责释放；在跨语言边界时，保持一致性（C 的 free 对应 C++ new 时要有桥接释放函数）。
   - 对订阅者回调使用弱引用或在 unsubscribe 时先禁止回调再释放对象，避免竞态条件（race）导致回调进入已释放对象。

9. 日志系统与后端
   - 提供统一的 `framework_log` API，后端可注册为 VOFA（串口）、UDP、文件等。后端实现需考虑线程安全与最小阻塞（将日志入队，由单独任务异步发送）。
   - 在高频 ISR/采样路径，避免直接调用串口阻塞发送，改为入环形缓冲并由底层任务处理。

10. 编译与验证流程
    - 本地构建：激活 IDF 环境 → `idf.py build`。
    - 若有链接或未定义符号，检查 component 路径、源文件扩展名及 `COMPONENT_SRCS`。
    - 若 C++ 类型信息（RTTI/异常）不一致，调整 `COMPONENT_ADD_INCLUDEDIRS` 与 `COMPONENT_PRIV_INCLUDEDIRS`，并同步 `CXXFLAGS`。

11. 测试与 CI
    - 在主机（PC）上为核心逻辑（PubSub、Factory、PID、Mixer）编写 GoogleTest 单元测试（提取为独立库以便在主机环境运行）。
    - 在 firmware 端使用 Unity/esp-idf unity 运行驱动级测试。
    - CI（GitHub Actions）分两阶段：host-tests（matrix: ubuntu）与 firmware-build（windows+mac+linux runner，或交叉编译容器）；在 firmware 步骤要先安装并激活 ESP-IDF。

## 关键注意事项（Precautions）
- C/C++ 边界：任何通过 C 回调传入 C++ 的指针，必须保证其生命周期在回调使用期间有效。
- 订阅资源回收：unsubscribe 必须同步或保证在分发任务中不再调用该回调（常用做法：先将句柄标记为无效、等待一个分发周期，然后释放）。
- 中断安全：在 ISR 中仅调用专为 ISR 设计的 API（如 `xQueueSendFromISR`），不要调用非线程/ISR 安全的库函数。
- 构件初始化顺序：避免在全局构造函数中注册运行时必需组件（在 component init 或明确 init 顺序中注册更可靠）。
- 编译器选项一致性：确保所有 C++ 源共享相同的 `-fno-exceptions` / `-fno-rtti` 设置，或显式启用所需特性并接受代价。

## 常见问题与排查建议
- 链接失败 undefined reference to `vtable for X`: 检查是否忘记实现纯虚函数或源文件未加入 component 编译。
- C++ 符号缺失：确认文件扩展名为 `.cpp` 且被添加到 COMPONENT_SRCS。
- 回调悬挂指针：复现步骤应包含频繁 subscribe/unsubscribe 场景，使用 ASAN 或静态分析工具在主机上先行检查。

## 简短检查清单（迁移每个模块时）
- [ ] 定义清晰的 C ABI（头文件 + 注释）。
- [ ] 提供 C++ Adapter，且不隐式释放 C 资源（除非明确约定）。
- [ ] 在 manager 中持有 PubSub 实例并注册 C bridge。
- [ ] 工厂注册在组件 init 时完成，且提供降级路径（shim）。
- [ ] 增加单元测试覆盖核心逻辑（非硬件依赖部分）。

## 参考
- ESP-IDF programming guide（组件化、CMake、FreeRTOS）
- FreeRTOS API：ISR-safe helper functions（xQueueSendFromISR 等）

----
如果你同意，我可以：
- 将本文件再翻译为简化版放入项目根 README，或
- 立即实现 PubSub 的订阅资源回收（在 `components/Framework/cpp/PubSub.*` 中添加句柄管理）。
