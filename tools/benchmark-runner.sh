#!/bin/bash
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
root_dir="$(cd "${script_dir}"/.. && pwd)"

bash "${root_dir}"/tools/build.sh

cd "${root_dir}"/build/benchmarks

echo "starting latency benchmark..."
#./all_in_one
echo "Latency benchmark done"

echo " "

echo "starting throuputh benchmark..."
echo "running with github_event data"
./json_parser_th data_github
echo "done data_github"
echo " "
echo "running benchmark with marine data"
./json_parser_th data_marine
echo "done data_marine"
echo " "
echo "running benchmark with a simdjson data"
./json_parser_th data_simdjson
echo "done simdjson_data"
echo "Throuput benchmark done"

echo " "

echo "strating memory consumption benchmark"
echo " note that this doesn't include gason"
echo " "
./auto_memory_bench data_github
echo "Memory consumption benchmark done"