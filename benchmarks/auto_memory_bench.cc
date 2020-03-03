#include <experimental/filesystem>
#include <vector>
#include <cstdio>
#include <string>

#include "memorystat.h"
#include "parsing_strategies.h"

namespace fs = std::experimental::filesystem;

using FN_TYPE = bool (*)(const char*);

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

int main(int argc, char *argv[]) {
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

    //TODOs:
    // validate arguments
    // set deufault path if none is provided
    // handle file argument (not only dirs)
    // write result into a file(json file maybe)

    for(const auto& entry : fs::directory_iterator(argv[1])) {
        if(entry.path().extension() == std::string{".json"}){
            for(const auto& elm : strategies){
                Memory::Instance().Reset();
                elm.second(entry.path().c_str());
                PrintMemoryStat();
                puts(elm.first);
                puts(entry.path().c_str());
                puts("\n");
            }
        }
    }

    return 0;
}