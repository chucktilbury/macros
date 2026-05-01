project(macros)

include(${PROJECT_SOURCE_DIR}/../../CMakeBuildOpts.txt)

add_executable(${PROJECT_NAME}
    define.c
    expression.c
    ifelse.c
    import.c
    main.c
    symbols.c
)

target_link_libraries(${PROJECT_NAME}
    gc
    cord
    common
)
