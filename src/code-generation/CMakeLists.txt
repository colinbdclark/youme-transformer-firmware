cmake_minimum_required(VERSION 3.13)
project(js-stdlib-generator C)

# Generate Micro QuickJS's Atoms Table
add_executable(generate-mquickjs-atoms
    ${MQUICKJS_PATH}/mqjs_stdlib.c
    ${MQUICKJS_PATH}/mquickjs_build.c
    ${MQUICKJS_PATH}/cutils.c
)
target_include_directories(generate-mquickjs-atoms PRIVATE ${MQUICKJS_PATH})
target_compile_options(generate-mquickjs-atoms PRIVATE -Wall -O2)

# Generate the JavaScript standard library bindings
add_executable(generate-js-stdlib
    generate-js-stdlib.c
    ${MQUICKJS_PATH}/mquickjs_build.c
    ${MQUICKJS_PATH}/cutils.c
)
target_include_directories(generate-js-stdlib PRIVATE ${MQUICKJS_PATH})
target_compile_options(generate-js-stdlib PRIVATE -Wall -O2)
