#include "parsing_strategies.h"


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

//nlohmann

bool nlohmannParse(const char* filename){
    auto [buffer, length] = open_file(filename);
    auto j = json::parse(buffer, nullptr, false);
    delete[] buffer;
    return j.is_discarded();
}