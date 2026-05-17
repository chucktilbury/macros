project(macros)

include(${PROJECT_SOURCE_DIR}/../../CMakeBuildOpts.txt)

add_executable(${PROJECT_NAME}
    ${PROJECT_NAME}.c
    process.c
    parms.c
    symbols.c
    define.c
    expression.c
    ifelse.c
    reference.c
    #context.c
)

target_link_libraries(${PROJECT_NAME}
    gc
    cord
    common
)
