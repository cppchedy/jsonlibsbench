#include <experimental/filesystem>
#include <vector>
#include <cstdio>
#include <string>
#include <numeric>

#include "memorystat.h"
#include "parsing_strategies.h"

namespace fs = std::experimental::filesystem;

using FN_TYPE = bool (*)(const char*);

static MemoryStat PrintMemoryStat() {
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
    return stat;
}

std::string memstat_to_json(MemoryStat stat, std::string file_name, uintmax_t fSize,  std::string parse_strg){
    return "{\n" 
        "\"mallocCount\" : " + std::to_string(stat.mallocCount) + "," +
        "\"reallocCount\" : " + std::to_string(stat.reallocCount) + "," +
        "\"freeCount\" : " + std::to_string(stat.freeCount) + "," +
        "\"peakSize\" : " + std::to_string(stat.peakSize) + "," +
        "\"fileName\" : \"" + file_name + "\" ," +
        "\"fileSize\" : " + std::to_string(fSize) + ","
        "\"parseStrategy\" : " + parse_strg +
        "\n}"; 
}

void dump_stats_to(const std::vector<std::string> &stats, const char* file_name){

    using namespace std::string_literals;
    std::string js_res = std::accumulate(std::begin(stats), std::end(stats), ""s);
    std::ofstream out;
    out.open(file_name);
    if(!out){
        puts("problem opening file");
        return ;
    }

    out << js_res;
    out.close();
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
    // set default path if none is provided
    // handle file argument (not only dirs)

    std::vector<std::string> memstats;
    memstats.reserve(16);

    for(const auto& entry : fs::directory_iterator(argv[1])) {
        auto size = fs::file_size(entry.path());
        if(entry.path().extension() == std::string{".json"}){
            for(const auto& elm : strategies){
                Memory::Instance().Reset();
                elm.second(entry.path().c_str());
                auto stat = PrintMemoryStat();
                auto json = memstat_to_json(
                    stat,
                    std::string{entry.path().c_str()},
                    size,
                    elm.first
                );
                
                puts(elm.first);
                puts(entry.path().c_str());
                puts("\n");
                memstats.push_back(json);
            }
        }
    }

    dump_stats_to(memstats, "memstat.json");

    return 0;
}