cmake_minimum_required(VERSION 3.7.1)
project(benchmark_jsonlibs VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(JSON_BuildTests OFF CACHE INTERNAL "")

add_definitions(-DSIMDJSON_BUILD_STATIC=ON)

add_compile_options(-O2)

add_subdirectory(thirdparty/benchmark)

add_subdirectory(thirdparty/json)
set(RAPIDJSON_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/rapidjson/include)
set(SAJSON_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/sajson/include)
set(GASON_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/gason/src)
add_subdirectory(thirdparty/simdjson)
add_subdirectory(thirdparty/gason)
include_directories(
    ${RAPIDJSON_INCLUDE_DIR}
    ${SAJSON_INCLUDE_DIR}
    ${GASON_INCLUDE_DIR}
)

enable_testing()
add_subdirectory(benchmarks)
file(COPY data/data_github DESTINATION ${CMAKE_BINARY_DIR}/benchmarks)
file(COPY data/data_marine DESTINATION ${CMAKE_BINARY_DIR}/benchmarks)
file(COPY data/data_simdjson DESTINATION ${CMAKE_BINARY_DIR}/benchmarks)
