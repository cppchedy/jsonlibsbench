#include <benchmark/benchmark.h>
#include <nlohmann/json.hpp>

#include <map>

using json = nlohmann::json;

std::map<int, std::string> files_github {
    { 4, "data_github/github_events_4K.json" },
    { 8, "data_github/github_events_8K.json"},
    { 16, "data_github/github_events_16K.json"},
    { 32, "data_github/github_events_32K.json"},
    { 64, "data_github/github_events_64K.json"},
    { 128, "data_github/github_events_128K.json"},
    { 256, "data_github/github_events_256K.json"},
    { 512, "data_github/github_events_512K.json"},
    { 1024, "data_github/github_events_1M.json"},
    { 2048, "data_github/github_events_2M.json"},
    { 4096, "data_github/github_events_4M.json"},
    { 8192, "data_github/github_events_8M.json"}
};

std::map<int, std::string> files_marine {
    { 4, "data_marine/marine_ik_4K.json" },
    { 8, "data_marine/marine_ik_8K.json"},
    { 16, "data_marine/marine_ik_16K.json"},
    { 32, "data_marine/marine_ik_32K.json"},
    { 64, "data_marine/marine_ik_64K.json"},
    { 128, "data_marine/marine_ik_128K.json"},
    { 256, "data_marine/marine_ik_256K.json"},
    { 512, "data_marine/marine_ik_512K.json"},
    { 1024, "data_marine/marine_ik_1M.json"},
    { 2048, "data_marine/marine_ik_2M.json"},
    { 4096, "data_marine/marine_ik_4M.json"},
    { 8192, "data_marine/marine_ik_8M.json"}
};

std::pair<char *, size_t> open_file(const char *file_name){
    FILE *file = fopen(file_name, "rb");
    if (!file){
        fprintf(stderr, "Failed to open file\n");
        return {};
    }
    fseek(file, 0, SEEK_END);
    size_t length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = new char[length];

    if (length != fread(buffer, 1, length, file)){
        fprintf(stderr, "Failed to read entire file\n");
        return {};
    }
    fclose(file);
    return {buffer, length};
}


static void BM_NlohmannParse(benchmark::State &state){
    auto [buffer, length] = open_file(files_github[state.range(0)].c_str());

    for (auto _ : state){
    auto j = json::parse(buffer, nullptr, false);
    }

    //printf("file %s has %d\n", files_github[state.range(0)].c_str(), i);

    delete[] buffer;
}

BENCHMARK(BM_NlohmannParse)->RangeMultiplier(2)->Range(4, 8192);

BENCHMARK_MAIN();

/*
g++-8 nlohmann_bench.cc -O2 -std=c++17 -fno-omit-frame-pointer -I/home/chedy/Documents/DevEnv/benchmark/include \
        -I/home/chedy/Documents/DevEnv/nlohmannjson/single_include -L/home/chedy/Documents/DevEnv/benchmark/build/src \
        -lbenchmark -pthread -o nlohmann_bench
*/