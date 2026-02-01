set(src_dirs
    ledPwm)

set(include_dirs
    ledPwm)

set(requires
    driver)

idf_component_register(SRC_DIRS ${src_dirs}
                       INCLUDE_DIRS ${include_dirs}
                       REQUIRES ${requires})

component_compile_options(PRIVATE -ffast-math -O3 -Wno-error=format -Wno-format)