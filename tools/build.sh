#!/bin/bash
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
root_dir="$(cd "${script_dir}"/.. && pwd)"

if ! [[ -e "${root_dir}"/build ]]; then
    mkdir -p "${root_dir}"/build
fi

cd "$root_dir"

cmake -B"${root_dir}"/build \
        -DBENCHMARK_ENABLE_TESTING=OFF \
        -H"${root_dir}"
cmake --build "${root_dir}"/build