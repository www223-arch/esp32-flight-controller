set(COMPONENT_DIR ${CMAKE_CURRENT_LIST_DIR})

# 注册框架子目录（源码按功能分层）
set(src_dirs
    hal
    drivers/imu
    middleware/ahrs
    control
    telemetry
    utils
    pubsub
    cpp
)

set(include_dirs
    hal
    drivers/imu
    middleware/ahrs
    control
    telemetry
    utils
    pubsub
    cpp
)

idf_component_register(SRC_DIRS ${src_dirs}
                       INCLUDE_DIRS ${include_dirs}
                       REQUIRES driver)

component_compile_options(PRIVATE -Wno-format)
