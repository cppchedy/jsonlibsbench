# Benchmarking test suite for json libraries

a benchmarking test suite for simdjson, sajson, rapidjson, gason and nlohmann json.

## build

```
git clone --recursive https://github.com/cppchedy/jsonlibsbench.git
cd jsonlibsbench
mkdir build
cmake -DBENCHMARK_ENABLE_TESTING=OFF -H. -Bbuild
cmake --build build
```

## run benchmarks
Still working on automating the run. Some binaries require you to be in data directory.

To Run the all_in_one latency benchmark:
```
#assuming you are in root dir.
cd data
../build/benchmarks/all_in_one
```

## single step

you can just run `tools/benchmark-runner.sh`. This script builds and runs the benchmarks.