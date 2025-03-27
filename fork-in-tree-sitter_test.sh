#!/bin/bash

set -e
trap 'echo "Test failed on line $LINENO"' ERR
trap tear_down EXIT

tear_down() {
    set +e
    rm fork-in fork-in-tree-sitter
}

gcc fork-in.c -o fork-in
gcc fork-in-tree-sitter.c \
    -I tree-sitter/lib/include \
    tree-sitter-json/src/parser.c \
    tree-sitter-cpp/src/parser.c \
    tree-sitter-cpp/src/scanner.c \
    tree-sitter-c/src/parser.c \
    tree-sitter-python/src/parser.c \
    tree-sitter-python/src/scanner.c \
    libtree-sitter.a \
    -o fork-in-tree-sitter

extensions=("json" "c" "cpp" "py")

for extension in "${extensions[@]}"; do
    if ./fork-in "examples/input.$extension" | \
            cmp -s - "examples/output_$extension.json"; then
        echo "Test $extension passed"
    else
        echo "Test $extension failed"
        exit 1
    fi
done
