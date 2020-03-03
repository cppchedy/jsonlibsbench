#include <unordered_map>

#include <simdjson/jsonparser.h>
#include <simdjson/jsonstream.h>
#include <rapidjson/document.h>
#include <sajson.h>

#include "utility.h"

using namespace simdjson;

using FN_TYPE = bool (*)(const char*);


//SIMDJson

bool simdjsonMBWRParse(const char* filename){
    auto [buffer, length] = open_file(filename);
    ParsedJson pj = build_parsed_json(buffer, length, false);

    delete[] buffer;

    return pj.is_valid();
}

bool simdjsonMBRParse(const char *filename) {
    auto [buffer, length] = open_file(filename);
    ParsedJson pj = build_parsed_json(buffer, length);

    delete[] buffer;

    return pj.is_valid();
}

bool simdjsonPStrParse(const char *filename) {
    padded_string p = get_corpus(filename);
    ParsedJson pj = simdjson::build_parsed_json(p);
    return pj.is_valid();
}

//Rapidjson

bool rapidjsonParseInsitu(const char *filename) {
    auto [buffer, length] = open_file(filename);
    
    rapidjson::Document d;
    d.ParseInsitu(buffer);

    delete[] buffer;
    return d.HasParseError();
}

bool rapidjsonParse(const char *filename) {
    auto [buffer, length] = open_file(filename);
    
    rapidjson::Document d;
    d.Parse(buffer);

    delete[] buffer;
    return d.HasParseError();
}

//Sajson

bool sajsonParseDynAlloc(const char *filename) {
    auto [buffer, length] = open_file(filename);
    const sajson::document document = sajson::parse(sajson::dynamic_allocation(), sajson::string(buffer, length));
    delete[] buffer;
    return document.is_valid();
}

bool sajsonParseSngAlloc(const char *filename) {
    auto [buffer, length] = open_file(filename);
    const sajson::document document = sajson::parse(sajson::single_allocation(), sajson::string(buffer, length));
    delete[] buffer;
    return document.is_valid();
}

bool sajsonParseDynAllocMutBuff(const char *filename) {
    auto [buffer, length] = open_file(filename);
    const sajson::document document = sajson::parse(sajson::dynamic_allocation(), sajson::mutable_string_view(length, buffer));
    delete[] buffer;
    return document.is_valid();
}

bool sajsonParseSngAllocMutBuff(const char *filename) {
    auto [buffer, length] = open_file(filename);
    const sajson::document document = sajson::parse(sajson::single_allocation(), sajson::mutable_string_view(length, buffer));
    delete[] buffer;
    return document.is_valid();
}

int main(int argc, char *argv[]) {
    std::unordered_map<std::string, FN_TYPE> funcs;
    funcs["simdMBWR"] = simdjsonMBWRParse;
    funcs["simdMBR"] = simdjsonMBRParse;
    funcs["simdPStr"] = simdjsonPStrParse;

    funcs["sajsonDA"] = sajsonParseDynAlloc;
    funcs["sajsonSA"] = sajsonParseSngAlloc;
    funcs["sajsonDAMB"] = sajsonParseDynAllocMutBuff;
    funcs["sajsonSAMB"] = sajsonParseSngAllocMutBuff;

    funcs["rapid"] = rapidjsonParse;
    funcs["rapidInsi"] = rapidjsonParseInsitu;

    const char * filename = argv[1];
    std::string fn{argv[2]};
    auto elm = funcs.find(fn);

    if(funcs.end() == elm) {
        puts("unknown method");
        return -1;
    }

    if(elm->second(filename))
        puts("valid");

    return 0;
}