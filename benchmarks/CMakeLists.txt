add_executable(json_parser json_parser.cc)
target_link_libraries(json_parser PRIVATE nlohmann_json::nlohmann_json
    gason
    simdjson
    PRIVATE nlohmann_json::nlohmann_json
)

add_executable(json_parser_bench json_parser_bench.cc)
target_link_libraries(json_parser_bench PRIVATE nlohmann_json::nlohmann_json
    gason
    simdjson
)


add_executable(json_parser_th json_parser_th.cc utility.cc)
target_link_libraries(json_parser_th simdjson stdc++fs)

add_executable(memory_bench memory_bench.cc)
target_link_libraries(memory_bench simdjson )

add_compile_options(-pthread)
add_executable(all_in_one single_gbench.cc utility.cc)
target_link_libraries(all_in_one benchmark::benchmark gason simdjson  nlohmann_json::nlohmann_json)

add_executable(gason_bench gason_bench.cc utility.cc)
target_link_libraries(gason_bench benchmark::benchmark gason simdjson)

add_executable(simdjson_bench simdjson_bench.cc utility.cc)
target_link_libraries(simdjson_bench benchmark::benchmark simdjson)

add_executable(nlohmann_bench nlohmann_bench.cc utility.cc)
target_link_libraries(nlohmann_bench benchmark::benchmark nlohmann_json::nlohmann_json)

add_executable(rapidjson_bench rapidjson_bench.cc utility.cc)
target_link_libraries(rapidjson_bench benchmark::benchmark)

add_executable(sajson_bench sajson_bench.cc utility.cc)
target_link_libraries(sajson_bench benchmark::benchmark )

