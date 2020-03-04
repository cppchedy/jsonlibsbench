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

int bench_dir_case(const std::vector<std::pair<const char *, FN_TYPE>> &strategies, const std::string &dir){
    std::vector<std::string> memstats;
    memstats.reserve(16);

    for(const auto& entry : fs::directory_iterator(dir)) {
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

int bench_file_case(const std::vector<std::pair<const char *, FN_TYPE>> &strategies, const std::string &file){
    auto fl = fs::path{file};
    auto size = fs::file_size(fl);

    std::vector<std::string> memstats;
    memstats.reserve(16);

    if (fl.extension() == std::string{".json"}){
        for (const auto &elm : strategies){
            Memory::Instance().Reset();
            elm.second(fl.c_str());
            auto stat = PrintMemoryStat();
            auto json = memstat_to_json(
                stat,
                file,
                size,
                elm.first
            );
            puts(elm.first);
            puts(fl.c_str());
            puts("\n");
            memstats.push_back(json);
        }
        dump_stats_to(memstats, "memstat.json");
        return 0;
    }
    puts("file name is required to end with .json");
    return -1;
}

int main(int argc, char *argv[]) {
    if(argv[1] == nullptr){
        puts("no path have been provided.");
        return -1;
    }
    std::string dir_or_file{argv[1]};

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

    strategies.emplace_back("nlohmannParse", nlohmannParse);

    fs::path arg{argv[1]} ; 
    if(fs::is_directory(arg))
        return bench_dir_case(strategies, dir_or_file);
    if(fs::is_regular_file(arg))
        return bench_file_case(strategies, dir_or_file);
    puts("invalid argument");
    return -1;

    //TODOs:
    // set default path if none is provided


    return 0;
}