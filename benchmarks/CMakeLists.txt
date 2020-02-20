add_executable(json_parser json_parser.cc memorystat.cpp)
target_link_libraries(json_parser PRIVATE nlohmann_json::nlohmann_json
    gasonmem
    simdjsonmemory
    PRIVATE nlohmann_json::nlohmann_json
)