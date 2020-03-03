#include <map>

#include <benchmark/benchmark.h>
#include <rapidjson/document.h>

#include "utility.h"

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

static void BM_RapidJsonParseInsitu(benchmark::State &state){
    auto [buffer, length] = open_file(files_github[state.range(0)].c_str());
    
    rapidjson::Document d;
    char* buff = new char[length];
    for (auto _ : state){
        state.PauseTiming();
        std::memcpy(buff, buffer, length);
        state.ResumeTiming();
        d.ParseInsitu(buff);
        benchmark::DoNotOptimize(d);
    }

    delete[] buffer;
    delete[] buff;
}

BENCHMARK(BM_RapidJsonParseInsitu)->RangeMultiplier(2)->Range(4, 8192);

static void BM_RapidJsonParse(benchmark::State &state){
    auto [buffer, length] = open_file(files_github[state.range(0)].c_str());
    
    rapidjson::Document d;
    for (auto _ : state){
        d.Parse(buffer);
        benchmark::DoNotOptimize(d);
    }

    delete[] buffer;
}
// Register the function as a benchmark
BENCHMARK(BM_RapidJsonParse)->RangeMultiplier(2)->Range(4, 8192);

BENCHMARK_MAIN();

/*
g++-8 rapidjson_bench.cc -O2 -std=c++17 -fno-omit-frame-pointer -I/home/chedy/Documents/DevEnv/benchmark/include -I/home/chedy/Documents/DevEnv/rapidjson/include   -L/home/chedy/Documents/DevEnv/benchmark/build/src -lbenchmark -lpthread -o rapidjson_bench
*/