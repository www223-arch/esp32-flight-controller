# 先获取当前 CMakeLists.txt 所在的目录
set(COMPONENT_DIR ${CMAKE_CURRENT_LIST_DIR})

# 源文件目录：用相对路径指向实际文件夹
set(src_dirs
    ${COMPONENT_DIR}/BSP/mytime   # mytime 在 BSP 下
    ${COMPONENT_DIR}/Middlewares/ps2  # ps2 在 Middlewares 下
    ${COMPONENT_DIR}/Bsp/Rmt  # ps2 在 Middlewares 下
)

# 头文件目录：同样用相对路径
set(include_dirs
    ${COMPONENT_DIR}/BSP/mytime
    ${COMPONENT_DIR}/Middlewares/ps2
    ${COMPONENT_DIR}/Bsp/Rmt  # ps2 在 Middlewares 下

)

# 依赖组件
set(requires
    driver
    led_strip  
)

idf_component_register(SRC_DIRS ${src_dirs}
                       INCLUDE_DIRS ${include_dirs}
                       REQUIRES ${requires})

component_compile_options(PRIVATE -ffast-math -O3 -Wno-error=format -Wno-format)