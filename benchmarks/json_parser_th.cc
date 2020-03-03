#include <experimental/filesystem>
#include <vector>
#include <algorithm>
#include <numeric>
#include <utility>
#include <iostream>
#include <fstream>

namespace fs = std::experimental::filesystem;

#include <simdjson/jsonparser.h>
#include <simdjson/jsonstream.h>
#include <rapidjson/document.h>
#include <sajson.h>

#include "utility.h"


using namespace simdjson;

struct bench_result_t {
    double lat;
    double throughput;
    bool valid_json;
    const char *bench_name;
    std::string file_name;
};

std::ostream& operator<<(std::ostream& out, const bench_result_t& b_res) {
  out << b_res.bench_name << " " << b_res.file_name << "\tLatency: " << b_res.lat <<"\tthrouput: " << b_res.throughput << '\n';
  return out;
}

using FN_TYPE = bench_result_t (*)(const char*, const char*);

bench_result_t simdjsonMBWRParse(const char* filename, const char* benchname){
    auto [buffer, length] = open_file(filename);

    auto start = std::chrono::steady_clock::now();
    ParsedJson pj = build_parsed_json(buffer, length, false);
    auto end = std::chrono::steady_clock::now();

    std::chrono::duration<double> secs = end - start;
    double speedinGBs = (length) / (secs.count() * 1000000000.0);

    bool valid = pj.is_valid();

    delete[] buffer;

    return bench_result_t{
      secs.count(),
      speedinGBs,
      valid,
      benchname,
      filename
    };
}

bench_result_t simdjsonMBRParse(const char* filename, const char* benchname){
    auto [buffer, length] = open_file(filename);

    auto start = std::chrono::steady_clock::now();
    ParsedJson pj = build_parsed_json(buffer, length);
    auto end = std::chrono::steady_clock::now();

    std::chrono::duration<double> secs = end - start;
    double speedinGBs = (length) / (secs.count() * 1000000000.0);

    bool valid = pj.is_valid();

    delete[] buffer;

    return bench_result_t{
      secs.count(),
      speedinGBs,
      valid,
      benchname,
      filename
    };
}

bench_result_t simdjsonPStrParse(const char *filename, const char* benchname) {
    padded_string p = get_corpus(filename);
    auto length = p.size();

    auto start = std::chrono::steady_clock::now();
    ParsedJson pj = simdjson::build_parsed_json(p);
    auto end = std::chrono::steady_clock::now();

    std::chrono::duration<double> secs = end - start;
    double speedinGBs = (length) / (secs.count() * 1000000000.0);

    bool valid = pj.is_valid();

    return bench_result_t{
      secs.count(),
      speedinGBs,
      valid,
      benchname,
      filename
    };
}

//Rapidjson

bench_result_t rapidjsonParseInsitu(const char *filename, const char* benchname) {
    auto [buffer, length] = open_file(filename);

    auto start = std::chrono::steady_clock::now();
    rapidjson::Document d;
    d.ParseInsitu(buffer);
    auto end = std::chrono::steady_clock::now();

    std::chrono::duration<double> secs = end - start;
    double speedinGBs = (length) / (secs.count() * 1000000000.0);

    bool valid = d.HasParseError();

    delete[] buffer;

    return bench_result_t{
      secs.count(),
      speedinGBs,
      valid,
      benchname,
      filename
    };
}

bench_result_t rapidjsonParse(const char *filename, const char* benchname) {
    auto [buffer, length] = open_file(filename);

    auto start = std::chrono::steady_clock::now();
    rapidjson::Document d;
    d.Parse(buffer);
    auto end = std::chrono::steady_clock::now();

    std::chrono::duration<double> secs = end - start;
    double speedinGBs = (length) / (secs.count() * 1000000000.0);

    bool valid = d.HasParseError();

    delete[] buffer;

    return bench_result_t{
      secs.count(),
      speedinGBs,
      valid,
      benchname,
      filename
    };
}

//Sajson

bench_result_t sajsonParseDynAlloc(const char *filename, const char* benchname) {
    auto [buffer, length] = open_file(filename);
    auto start = std::chrono::steady_clock::now();
    const sajson::document document = sajson::parse(sajson::dynamic_allocation(), sajson::string(buffer, length));
    auto end = std::chrono::steady_clock::now();

    std::chrono::duration<double> secs = end - start;
    double speedinGBs = (length) / (secs.count() * 1000000000.0);

    bool valid = document.is_valid();

    delete[] buffer;

    return bench_result_t{
      secs.count(),
      speedinGBs,
      valid,
      benchname,
      filename
    };
}

bench_result_t sajsonParseSngAlloc(const char *filename, const char* benchname) {
    auto [buffer, length] = open_file(filename);
    auto start = std::chrono::steady_clock::now();
    const sajson::document document = sajson::parse(sajson::single_allocation(), sajson::string(buffer, length));
     auto end = std::chrono::steady_clock::now();

    std::chrono::duration<double> secs = end - start;
    double speedinGBs = (length) / (secs.count() * 1000000000.0);

    bool valid = document.is_valid();

    delete[] buffer;

    return bench_result_t{
      secs.count(),
      speedinGBs,
      valid,
      benchname,
      filename
    };
}

bench_result_t sajsonParseDynAllocMutBuff(const char *filename, const char* benchname) {
    auto [buffer, length] = open_file(filename);
    auto start = std::chrono::steady_clock::now();
    const sajson::document document = sajson::parse(sajson::dynamic_allocation(), sajson::mutable_string_view(length, buffer));
    auto end = std::chrono::steady_clock::now();

    std::chrono::duration<double> secs = end - start;
    double speedinGBs = (length) / (secs.count() * 1000000000.0);

    bool valid = document.is_valid();

    delete[] buffer;

    return bench_result_t{
      secs.count(),
      speedinGBs,
      valid,
      benchname,
      filename
    };
}

bench_result_t sajsonParseSngAllocMutBuff(const char *filename, const char* benchname) {
    auto [buffer, length] = open_file(filename);
    auto start = std::chrono::steady_clock::now();
    const sajson::document document = sajson::parse(sajson::single_allocation(), sajson::mutable_string_view(length, buffer));
    auto end = std::chrono::steady_clock::now();

    std::chrono::duration<double> secs = end - start;
    double speedinGBs = (length) / (secs.count() * 1000000000.0);

    bool valid = document.is_valid();

    delete[] buffer;

    return bench_result_t{
      secs.count(),
      speedinGBs,
      valid,
      benchname,
      filename
    };
}

void dump_to_json(const std::vector<bench_result_t>& in, const char* file_name){
    auto bench_to_json = [](const bench_result_t& b) -> std::string {
        return "{\n \"throughput\" :" + std::to_string(b.throughput) + "," +
                    "\"latency\" : " + std::to_string(b.lat) + "," +
                    "\"filename\" : \"" + b.file_name + "\"," +
                    "\"benchname\" : \"" + b.bench_name + "\""
        "\n}";
    };

    std::vector<std::string> bench_jsonified;
    bench_jsonified.reserve(in.size());


    std::transform(std::begin(in), std::end(in), std::back_inserter(bench_jsonified), 
        [&bench_to_json](const auto& e){
            return bench_to_json(e);
        });

    using namespace std::string_literals;
    std::string js_res = 
        std::accumulate(std::begin(bench_jsonified), std::end(bench_jsonified)-1, "["s, 
            [](std::string acc, std::string str){
                return acc  + str + ", ";
        }) + *(std::end(bench_jsonified)-1) + "]";

    std::ofstream out;
    out.open(file_name);
    if(!out){
        puts("problem opening file");
        return ;
    }

    out << js_res;
    out.close();
}

int main(int argc, char* argv[]) {
    std::vector<std::pair<const char *, FN_TYPE>> strategies;
    strategies.reserve(16);

    strategies.emplace_back("simdjsonMBWRParse", simdjsonMBWRParse);
    strategies.emplace_back("simdjsonMBRParse", simdjsonMBRParse);
    strategies.emplace_back("simdjsonPStrParse", simdjsonPStrParse);

    strategies.emplace_back("rapidjsonParseInsitu", rapidjsonParseInsitu);
    strategies.emplace_back("rapidjsonParse", rapidjsonParse);

    strategies.emplace_back("sajsonParseDynAlloc", sajsonParseDynAlloc);
    strategies.emplace_back("sajsonParseSngAlloc", sajsonParseSngAlloc);
    strategies.emplace_back("sajsonParseDynAllocMutBuff", sajsonParseDynAllocMutBuff);
    strategies.emplace_back("sajsonParseSngAllocMutBuff", sajsonParseSngAllocMutBuff);

    std::vector<bench_result_t> results;
    results.reserve(strategies.size());

    for(auto &entry : fs::directory_iterator(argv[1])){        
        if( entry.path().extension() == std::string{".json"}){
            std::for_each(std::begin(strategies), std::end(strategies), 
            [&results, &entry](auto& bench){
                results.push_back(bench.second(entry.path().c_str(), bench.first));
            });
        }
    }

    std::for_each(std::begin(results), std::end(results), [](auto &elm) {
        std::cout << elm;
    });

    dump_to_json(results, "out.json");
    return 0;
}
