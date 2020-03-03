#include <benchmark/benchmark.h>
#include <sajson.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <utility>
#include <map>

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

static void BM_SajsonParseDynAlloc(benchmark::State &state){
    auto [buffer, length] = open_file(files_github[state.range(0)].c_str());

    for (auto _ : state){
        const sajson::document document = sajson::parse(sajson::dynamic_allocation(), sajson::string(buffer, length));
        benchmark::DoNotOptimize(document);
    }

    delete[] buffer;
}

BENCHMARK(BM_SajsonParseDynAlloc)->RangeMultiplier(2)->Range(4, 8192);

static void BM_SajsonParseSingleAlloc(benchmark::State &state){
    auto [buffer, length] = open_file(files_github[state.range(0)].c_str());

    for (auto _ : state){
        const sajson::document document = sajson::parse(sajson::single_allocation(), sajson::string(buffer, length));
        benchmark::DoNotOptimize(document);
    }

    delete[] buffer;
}

BENCHMARK(BM_SajsonParseSingleAlloc)->RangeMultiplier(2)->Range(4, 8192);

static void BM_SajsonParseMutableBuffDynAlloc(benchmark::State &state){
    auto [buffer, length] = open_file(files_github[state.range(0)].c_str());

    char* buff = new char[length];
    for (auto _ : state){
        state.PauseTiming();
        std::memcpy(buff, buffer, length);
        state.ResumeTiming();
        const sajson::document document = sajson::parse(sajson::dynamic_allocation(), sajson::mutable_string_view(length, buff));
        benchmark::DoNotOptimize(document);
    }

    delete[] buffer;
    delete[] buff;
}

BENCHMARK(BM_SajsonParseMutableBuffDynAlloc)->RangeMultiplier(2)->Range(4, 8192);

static void BM_SajsonParseMutableBuffSingleAlloc(benchmark::State &state){
    auto [buffer, length] = open_file(files_github[state.range(0)].c_str());

    char* buff = new char[length];
    for (auto _ : state){
        state.PauseTiming();
        std::memcpy(buff, buffer, length);
        state.ResumeTiming();
        const sajson::document document = sajson::parse(sajson::single_allocation(), sajson::mutable_string_view(length, buff));
        benchmark::DoNotOptimize(document);
    }

    delete[] buffer;
    delete[] buff;
}

BENCHMARK(BM_SajsonParseMutableBuffSingleAlloc)->RangeMultiplier(2)->Range(4, 8192);

BENCHMARK_MAIN();

/*
g++-8 sajson_bench.cc -O2 -std=c++17 -fno-omit-frame-pointer -I/home/chedy/Documents/DevEnv/benchmark/include -I/home/chedy/Documents/DevEnv/sajson/include -L/home/chedy/Documents/DevEnv/benchmark/build/src -lbenchmark -pthread -o sajson_bench
*/