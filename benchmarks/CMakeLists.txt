add_executable(json_parser json_parser.cc memorystat.cpp)
target_link_libraries(json_parser PRIVATE nlohmann_json::nlohmann_json
    gasonmem
    simdjsonmemory
    PRIVATE nlohmann_json::nlohmann_json
)

add_executable(json_parser_bench json_parser_bench.cc)
target_link_libraries(json_parser_bench PRIVATE nlohmann_json::nlohmann_json
    gason
    simdjson
)
