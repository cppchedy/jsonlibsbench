#include "memorystat.h"

#include <simdjson/jsonparser.h>
#include <simdjson/jsonstream.h>

#include <rapidjson/document.h>
#include <nlohmann/json.hpp>
#include <sajson.h>
#include <gason.h>

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string_view>
#include <chrono>

#include <sys/resource.h>

static void PrintMemoryStat() {
    const MemoryStat& stat = Memory::Instance().GetStat();
    printf(
        "Memory stats:\n"
        " mallocCount = %u\n"
        "reallocCount = %u\n"
        "   freeCount = %u\n"
        " currentSize = %u\n"
        "    peakSize = %u\n",
        (unsigned)stat.mallocCount,
        (unsigned)stat.reallocCount,
        (unsigned)stat.freeCount,
        (unsigned)stat.currentSize,
        (unsigned)stat.peakSize);
}

bool parse_file_simdjson_stream(const char *filename) {
    simdjson::padded_string p = simdjson::get_corpus(filename);
    simdjson::ParsedJson pj;
    simdjson::JsonStream js{p};
    int parse_res = simdjson::SUCCESS_AND_HAS_MORE;
    
    while (parse_res == simdjson::SUCCESS_AND_HAS_MORE) {
            parse_res = js.json_parse(pj);
        }
    return pj.is_valid();
}

bool parse_file_simdjson(const char *filename){
    simdjson::padded_string p = simdjson::get_corpus(filename);
    auto length = p.size();
    auto start = std::chrono::steady_clock::now();
    simdjson::ParsedJson pj = build_parsed_json(p);
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> secs = end - start;
    double speedinGBs = (length) / (secs.count() * 1000000000.0);
    std::cout << "simdjson : speedinGbs\t\t\t\t" << speedinGBs << "\t\t\t\tlatency :" << secs.count() << std::endl;
    PrintMemoryStat();
    return pj.is_valid();
}

int parse_file_simdjson_allocate_then_parse(const char *filename){
    simdjson::padded_string p;
    simdjson::get_corpus(filename).swap(p);
    auto length = p.size();
    
    simdjson::ParsedJson pj;
    pj.allocate_capacity(
        p.size()
        );
    auto start = std::chrono::steady_clock::now();
    auto x = json_parse(p, pj);
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> secs = end - start;
    double speedinGBs = (length) / (secs.count() * 1000000000.0);
    std::cout << speedinGBs << "\t\t\t\t" << secs.count() << std::endl;
    return x;
}

bool parse_sajson(const char *filename){
        FILE* file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Failed to open file\n");
        return false;
    }
    fseek(file, 0, SEEK_END);
    size_t length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = new char[length];
    if (length != fread(buffer, 1, length, file)) {
        fprintf(stderr, "Failed to read entire file\n");
        return false;
    }
    fclose(file);
    
    auto start = std::chrono::steady_clock::now();
    const sajson::document document = sajson::parse(sajson::dynamic_allocation(), sajson::mutable_string_view(length, buffer));
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> secs = end - start;
    double speedinGBs = (length) / (secs.count() * 1000000000.0);
    std::cout << "sajson : speedinGbs\t\t\t\t" << speedinGBs << "\t\t\t\tlatency :" << secs.count() << std::endl;
    delete[] buffer;
    PrintMemoryStat();
    return document.is_valid();
}

bool parse_rapid(const char *filename) {
    simdjson::padded_string p;
    simdjson::get_corpus(filename).swap(p);
    auto length = p.size();
    rapidjson::Document d;
    auto start = std::chrono::steady_clock::now();
    d.Parse(p.data());
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> secs = end - start;
    double speedinGBs = (length) / (secs.count() * 1000000000.0);
    std::cout << "rapid : speedinGbs\t\t\t\t" << speedinGBs << "\t\t\t\tlatency :" << secs.count() << std::endl;
    PrintMemoryStat();
    return d.HasParseError();
}

bool parse_rapid_insitu(const char *filename) {
    simdjson::padded_string p;
    simdjson::get_corpus(filename).swap(p);
    auto length = p.size();
    rapidjson::Document d;
    auto start = std::chrono::steady_clock::now();
    d.ParseInsitu(p.data());
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> secs = end - start;
    double speedinGBs = (length) / (secs.count() * 1000000000.0);
    std::cout << "rapid-ins : speedinGbs\t\t\t\t" << speedinGBs << "\t\t\t\tlatency :" << secs.count() << std::endl;
    PrintMemoryStat();
    return d.HasParseError();
}

bool parse_gason(const char *filename) {
    simdjson::padded_string p;
    simdjson::get_corpus(filename).swap(p);
    auto length = p.size();
    char *endptr;
    JsonValue value;
    JsonAllocator allocator;
    auto start = std::chrono::steady_clock::now();
    int status = jsonParse(p.data(), &endptr, &value, allocator);
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> secs = end - start;
    double speedinGBs = (length) / (secs.count() * 1000000000.0);
    std::cout << "gason : speedinGbs\t\t\t\t" << speedinGBs << "\t\t\t\tlatency :" << secs.count() << std::endl;
    PrintMemoryStat();
    return status;
}

bool parse_nlohmann_json(const char *filename){
    simdjson::padded_string p;
    simdjson::get_corpus(filename).swap(p);
    auto length = p.size();
    auto start = std::chrono::steady_clock::now();
    auto doc = nlohmann::json::parse(p.data());
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> secs = end - start;
    double speedinGBs = (length) / (secs.count() * 1000000000.0);
    std::cout << "nlohmann : speedinGbs\t\t\t\t" << speedinGBs << "\t\t\t\tlatency :" << secs.count() << std::endl;
    PrintMemoryStat();
    return doc.is_discarded();
}

int main(int argc, char *argv[]) {
    const char * filename = argv[1];
    std::string_view fn{argv[2]};
    if(fn == "all"){
        parse_rapid_insitu(filename); 
        parse_gason(filename);
        parse_nlohmann_json(filename);
        parse_file_simdjson(filename);
        parse_sajson(filename);
        parse_rapid(filename);
        return 0;
    }
    if(fn == "simd-stream") {
        bool ok = parse_file_simdjson_stream(filename);
        PrintMemoryStat();
        return ok;
    }
    else if(fn == "simd"){
        bool ok = parse_file_simdjson(filename);
        PrintMemoryStat();
        return ok;
    }
    else if(fn == "simd2"){
        bool ok = parse_file_simdjson_allocate_then_parse(filename);
        PrintMemoryStat();
        return ok;
    }
    else if(fn == "sajson"){
        bool ok = parse_sajson(filename);
        PrintMemoryStat();
        return ok;
    }
    else if(fn == "rapid"){
        bool ok = parse_rapid(filename);
        PrintMemoryStat();
        return ok;
    }
    else if(fn == "rapid-insitu"){
        bool ok = parse_rapid_insitu(filename);
        PrintMemoryStat();
        return ok;
    }
    else if(fn == "gason"){
        bool ok = parse_gason(filename);
        PrintMemoryStat();
        return ok;
    }
    else if(fn == "nlohmann"){
        bool ok = parse_nlohmann_json(filename);
        PrintMemoryStat();
        return ok;
    }
    else
        puts("unknow method");
    return 0;
}
