#include <chrono>
#include <vector>
#include <utility>
#include <memory>
#include <variant>
#include <algorithm>

#include <gason.h>
#include <sajson.h>
#include <nlohmann/json.hpp>
#include <rapidjson/document.h>
#include <simdjson/jsonparser.h>

using json = nlohmann::json;

struct bench_result_t {
    double lat;
    double throughput;
    bool valid_json;
    const char *bench_name;
    const char *file_name;
};

std::ostream& operator<<(std::ostream& out, const bench_result_t& b_res) {
  out << b_res.bench_name << "/" << b_res.file_name << "\tLatency: " << b_res.lat <<"\tthrouput: " << b_res.throughput << '\n';
  return out;
}

struct parser {
  std::vector<const char*> files;
  const char *bench_name;
  parser(std::vector<const char*> f, const char* name) 
    : files{f}, bench_name{name} 
    { }
};

struct rapidjson_parser : parser {
  using parser::parser;
  inline static bool parse(simdjson::padded_string buff){
    rapidjson::Document d;
    return d.ParseInsitu(buff.data()).HasParseError();
  }

  inline static std::pair<simdjson::padded_string, size_t>
  load(const char *filename){
    auto p = simdjson::get_corpus(filename);
    return std::make_pair(std::move(p), p.size());
  }
};

struct gason_parser : parser {
  using parser::parser;
  inline static bool parse(simdjson::padded_string buff){
    char *endptr;
    JsonValue value;
    JsonAllocator allocator;
    return jsonParse(buff.data(), &endptr, &value, allocator);
  }

  inline static std::pair<simdjson::padded_string, size_t>
  load(const char *filename){
    auto p = simdjson::get_corpus(filename);
    return std::make_pair(std::move(p), p.size());
  }
};

struct sajson_parser : parser{
  using parser::parser;
  inline static bool parse(simdjson::padded_string buff){
    const sajson::document document = sajson::parse(sajson::single_allocation(), 
                                                    sajson::mutable_string_view(buff.size(), buff.data()));
    return document.is_valid();
  }

  inline static std::pair<simdjson::padded_string, size_t>
  load(const char *filename){
    auto p = simdjson::get_corpus(filename);
    return std::make_pair(std::move(p), p.size());
  }
};

struct simdjson_parser : parser {
  using parser::parser;
  inline static bool parse(simdjson::padded_string p) {
    simdjson::ParsedJson pj = build_parsed_json(p);
    return pj.is_valid();
  }

  inline static std::pair<simdjson::padded_string, size_t>
  load(const char *filename) {
    auto p = simdjson::get_corpus(filename);
    return std::make_pair(std::move(p), p.size());
  }
};

struct nlohmannjson_parser : parser { 
  using parser::parser;
  inline static bool parse(simdjson::padded_string p) {
    auto j = json::parse(p.data(), nullptr, false);
    return j.is_discarded();
  }

  inline static std::pair<simdjson::padded_string, size_t>
  load(const char *filename) {
    auto p = simdjson::get_corpus(filename);
    return std::make_pair(std::move(p), p.size());
  }
};

template <class Parser>
bench_result_t benchmark(const char *benchname, const char *filename) {
  auto[buffer, length] = std::remove_reference<Parser>::type::load(filename);
  //idea benchmark policy: no copy, copy
  //factory function specialization of true and false.
  // add a constexpr static member for the parser to indicate if the parsing is in-place or use extra mem.
  auto start = std::chrono::steady_clock::now();
  bool valid = std::remove_reference<Parser>::type::parse(std::move(buffer));
  auto end = std::chrono::steady_clock::now();
  std::chrono::duration<double> secs = end - start;
  return bench_result_t{
      secs.count(),
      (length) / (secs.count() * 1000000000.0),
      valid,
      benchname,
      filename
    };
}

#define MODE 1
#define DIRORFILE 2

int main(int argc, char *argv[]) {
  if(argc < 2){
    puts("assuming usage of default data set");
    std::vector<const char *> github_files {  "data_github/github_events_4K.json",
                                          "data_github/github_events_8K.json",
                                          "data_github/github_events_16K.json",
                                          "data_github/github_events_32K.json",
                                          "data_github/github_events_64K.json",
                                          "data_github/github_events_128K.json",
                                          "data_github/github_events_256K.json",
                                          "data_github/github_events_512K.json",
                                          "data_github/github_events_1M.json",
                                          "data_github/github_events_2M.json",
                                          "data_github/github_events_4M.json",
                                          "data_github/github_events_8M.json" 
    };

    std::vector<const char *> marine_files{"data_marine/marine_ik_4K.json",
                                            "data_marine/marine_ik_8K.json",
                                            "data_marine/marine_ik_16K.json",
                                            "data_marine/marine_ik_32K.json",
                                            "data_marine/marine_ik_64K.json",
                                            "data_marine/marine_ik_128K.json",
                                            "data_marine/marine_ik_256K.json",
                                            "data_marine/marine_ik_512K.json",
                                            "data_marine/marine_ik_1M.json",
                                            "data_marine/marine_ik_2M.json",
                                            "data_marine/marine_ik_4M.json",
                                            "data_marine/marine_ik_8M.json"
                                            };

    auto start_benchmark = [](auto &&... args) {
      auto wrapper = [](auto &arg, auto &results) {
        std::for_each(arg.files.begin(), arg.files.end(), [arg, &results](auto x) {
          results.emplace_back(benchmark<decltype(arg)>(arg.bench_name, x));
        });
        results.emplace_back("\t\t --------- \t\t\n");
      };
      std::vector<std::variant<bench_result_t, const char*>> results;
      results.reserve( (args.files.size() + ...));
      ( wrapper(args, results), ...);
      return results;
    };

    simdjson_parser sp_github{github_files, "simdjson_parser github_files"};
    simdjson_parser sp_marine{marine_files, "simdjson_parser marine_files"};

    sajson_parser sjon_github{github_files, "sajson_parser github_files"};
    sajson_parser sjon_marine{marine_files, "sajson_parser marine_files"};

    gason_parser g_github{github_files, "gason_parser github_files"};
    gason_parser g_marine{marine_files, "gason_parser marine_files"};

    nlohmannjson_parser n_github{github_files, "nlohmann_parser github_files"};
    nlohmannjson_parser n_marine{marine_files, "nlohmann_parser marine_files"};

    rapidjson_parser rapd_github{github_files, "rapidjson_parser github_files"};
    rapidjson_parser rapd_marine{marine_files, "rapidjson_parser marine_files"};


    auto res = start_benchmark(sp_github, sp_marine, sjon_github, sjon_marine,
    g_github, g_marine, n_github, n_marine, rapd_github, rapd_marine);

    std::for_each(std::begin(res), std::end(res), [](auto x){
      std::visit([](auto&& in) { std::cout << in;}, x);
    });

  } else {
  std::string_view mode{argv[MODE]};
  }


}

/*
g++-8 json_parser_bench.cc -std=c++17 -O2 -I/home/chedy/Documents/DevEnv/simdjson/include \
      -I/home/chedy/Documents/DevEnv/simdjson/src -I/home/chedy/Documents/DevEnv/sajson/include \
      -I/home/chedy/Documents/DevEnv/nlohmannjson/single_include -I/home/chedy/Documents/DevEnv/gason/src \
      -I/home/chedy/Documents/DevEnv/rapidjson/include                                    \
      -L/home/chedy/Documents/DevEnv/simdjson/build/src -L/home/chedy/Documents/DevEnv/gason/build \
      -lsimdjson -lgason -o json_parser_bench
*/