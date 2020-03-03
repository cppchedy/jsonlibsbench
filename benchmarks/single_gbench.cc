#include <map>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <utility>

#include <benchmark/benchmark.h>
#include <simdjson/jsonparser.h>
#include <simdjson/jsonstream.h>
#include <rapidjson/document.h>
#include <nlohmann/json.hpp>
#include <sajson.h>
#include <gason.h>

#include "utility.h"

using namespace simdjson;
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

#define TEST_FILES files_github


static void BM_SajsonParseDynAlloc(benchmark::State &state){
    auto [buffer, length] = open_file(TEST_FILES[state.range(0)].c_str());

    for (auto _ : state){
        const sajson::document document = sajson::parse(sajson::dynamic_allocation(), sajson::string(buffer, length));
        benchmark::DoNotOptimize(document);
    }

    delete[] buffer;
}

BENCHMARK(BM_SajsonParseDynAlloc)->RangeMultiplier(2)->Range(4, 8192);

static void BM_SajsonParseSingleAlloc(benchmark::State &state){
    auto [buffer, length] = open_file(TEST_FILES[state.range(0)].c_str());

    for (auto _ : state){
        const sajson::document document = sajson::parse(sajson::single_allocation(), sajson::string(buffer, length));
        benchmark::DoNotOptimize(document);
    }

    delete[] buffer;
}

BENCHMARK(BM_SajsonParseSingleAlloc)->RangeMultiplier(2)->Range(4, 8192);

static void BM_SajsonParseMutableBuffDynAlloc(benchmark::State &state){
    auto [buffer, length] = open_file(TEST_FILES[state.range(0)].c_str());

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
    auto [buffer, length] = open_file(TEST_FILES[state.range(0)].c_str());

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

static void BM_SIMDJsonParseRawBuffWithoutRelo(benchmark::State &state){
    auto [buffer, length] = open_file(TEST_FILES[state.range(0)].c_str());
    ParsedJson pj;
    for (auto _ : state){
        pj = build_parsed_json(buffer, length, false);
        benchmark::DoNotOptimize(pj);
    }
    delete[] buffer;
}

BENCHMARK(BM_SIMDJsonParseRawBuffWithoutRelo)->RangeMultiplier(2)->Range(4, 8192);

static void BM_SIMDJsonParseRawBuffReallo(benchmark::State &state){
    auto [buffer, length] = open_file(TEST_FILES[state.range(0)].c_str());
    ParsedJson pj;
    for (auto _ : state){
        pj = build_parsed_json(buffer, length);
        benchmark::DoNotOptimize(pj);
    }
    delete[] buffer;
}

BENCHMARK(BM_SIMDJsonParseRawBuffReallo)->RangeMultiplier(2)->Range(4, 8192);

static void BM_SIMDJsonParsePaddedString(benchmark::State &state){
    padded_string p = get_corpus(TEST_FILES[state.range(0)]);
    ParsedJson pj;
    for (auto _ : state){
        pj = build_parsed_json(p);
        benchmark::DoNotOptimize(pj);
    }
}

BENCHMARK(BM_SIMDJsonParsePaddedString)->RangeMultiplier(2)->Range(4, 8192);

static void BM_RapidJsonParseInsitu(benchmark::State &state){
    auto [buffer, length] = open_file(TEST_FILES[state.range(0)].c_str());
    
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
    auto [buffer, length] = open_file(TEST_FILES[state.range(0)].c_str());
    
    rapidjson::Document d;
    for (auto _ : state){
        d.Parse(buffer);
        benchmark::DoNotOptimize(d);
    }

    delete[] buffer;
}

BENCHMARK(BM_RapidJsonParse)->RangeMultiplier(2)->Range(4, 8192);

//Nlohmann
static void BM_NlohmannParse(benchmark::State &state){
    auto [buffer, length] = open_file(TEST_FILES[state.range(0)].c_str());

    for (auto _ : state){
    auto j = json::parse(buffer, nullptr, false);
    }

    //printf("file %s has %d\n", TEST_FILES[state.range(0)].c_str(), i);

    delete[] buffer;
}

BENCHMARK(BM_NlohmannParse)->RangeMultiplier(2)->Range(4, 8192);


static void BM_GasonParse(benchmark::State &state){
    auto [buffer, length] = open_file(TEST_FILES[state.range(0)].c_str());
    
    char* buff = new char[length];
    char *endptr;
    JsonValue value;
    JsonAllocator allocator;
    for (auto _ : state){
        state.PauseTiming();
        std::memcpy(buff, buffer, length);
        state.ResumeTiming();
        int status = jsonParse(buff, &endptr, &value, allocator);
        benchmark::DoNotOptimize(status);
    }

    delete[] buffer;
    delete[] buff;
}

BENCHMARK(BM_GasonParse)->RangeMultiplier(2)->Range(4, 8192);

BENCHMARK_MAIN();
