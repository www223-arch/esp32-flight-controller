# 先获取当前 CMakeLists.txt 所在的目录
set(COMPONENT_DIR ${CMAKE_CURRENT_LIST_DIR})

# 源文件目录：指向文件夹（里面的vofa.c会被自动识别）
set(src_dirs
    ${COMPONENT_DIR}/BSP/mytime          # mytime 文件夹（BSP下）
    ${COMPONENT_DIR}/Middlewares/PS2     # PS2 文件夹（Middlewares下）
    ${COMPONENT_DIR}/BSP/Rmt             # Rmt 文件夹（BSP下）
    ${COMPONENT_DIR}/BSP/myuart          # myuart 文件夹（BSP下）
    ${COMPONENT_DIR}/Middlewares/MyVofa  # MyVofa 文件夹（Middlewares下，里面有vofa.c）
)

# 头文件目录：和源文件目录对应（指向文件夹，找vofa.h）
set(include_dirs
    ${COMPONENT_DIR}/BSP/mytime
    ${COMPONENT_DIR}/Middlewares/PS2
    ${COMPONENT_DIR}/BSP/Rmt
    ${COMPONENT_DIR}/BSP/myuart
    ${COMPONENT_DIR}/Middlewares/MyVofa
)

# 依赖组件（driver/led_strip）
set(requires
    driver
    led_strip
)

# 注册组件（核心）
idf_component_register(SRC_DIRS ${src_dirs}
                       INCLUDE_DIRS ${include_dirs}
                       REQUIRES ${requires})

# 原有编译选项（保留）
component_compile_options(PRIVATE -ffast-math -O3 -Wno-error=format -Wno-format)