project(common)

include(${PROJECT_SOURCE_DIR}/../../CMakeBuildOpts.txt)

add_library(${PROJECT_NAME} STATIC
    alloc.c
    fileio.c
    pointer_list.c
    string_list.c
    string_buffer.c
    char_buffer.c
    trace.c
    cmdline.c
    error.c
)

add_definitions( -DUSE_GC )
include_directories(${PROJECT_SOURCE_DIR}/gc/include)
add_subdirectory(gc)
