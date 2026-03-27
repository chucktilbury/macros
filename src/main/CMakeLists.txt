project(macros)

include(${PROJECT_SOURCE_DIR}/../../CMakeBuildOpts.txt)

find_package(FLEX 2.6 REQUIRED)
find_package(BISON 3.8 REQUIRED)

BISON_TARGET(PARSER
    parser.y
    ${CMAKE_CURRENT_BINARY_DIR}/parser.c
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.h
    COMPILE_FLAGS ""
)

FLEX_TARGET(SCANNER
    scanner.l
    ${CMAKE_CURRENT_BINARY_DIR}/scanner.c
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/scanner.h
    COMPILE_FLAGS -i
)

ADD_FLEX_BISON_DEPENDENCY(SCANNER PARSER)

add_executable(${PROJECT_NAME}
    ${BISON_PARSER_OUTPUTS}
    ${FLEX_SCANNER_OUTPUTS}
    symbols.c
    fileio.c
    main.c
)

target_link_libraries(${PROJECT_NAME}
    gc
    cord
    common
)
