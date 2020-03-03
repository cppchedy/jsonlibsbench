#include <unordered_map>

#include "parsing_strategies.h"
#include "utility.h"



using FN_TYPE = bool (*)(const char*);




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