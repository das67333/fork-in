#!/bin/bash

set -e
trap 'echo "Test failed on line $LINENO"' ERR
trap tear_down EXIT

tear_down() {
    set +e
    rm fork-in "${backends[@]}"
}

backends=("fork-in-lsp" "fork-in-dt" "fork-in-regex")

for backend in "${backends[@]}"; do
    gcc -x c -o $backend - << EOF
    #include <stdio.h>
    int main() { printf("Using $backend backend\n"); }
EOF
done

gcc fork-in.c -o fork-in
./fork-in filename.go
./fork-in filename.lsp
./fork-in filename.md
