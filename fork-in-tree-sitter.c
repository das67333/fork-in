#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tree_sitter/api.h>

const TSLanguage* tree_sitter_json(void);
const TSLanguage* tree_sitter_cpp(void);
const TSLanguage* tree_sitter_c(void);
const TSLanguage* tree_sitter_python(void);

char* read_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = (char*)malloc(size + 1);
    if (buffer == NULL) {
        perror("Memory allocation failed");
        fclose(file);
        exit(1);
    }

    size_t bytes_read = fread(buffer, 1, size, file);
    if (bytes_read != size) {
        perror("Error reading file");
        free(buffer);
        fclose(file);
        exit(1);
    }
    buffer[size] = '\0';

    fclose(file);
    return buffer;
}

char* encode_string(const char* s) {
    if (s == NULL) {
        fprintf(stderr, "Called encode_string with NULL\n");
        exit(1);
    }

    const char symbols_to_escape[] = {'\"', '\\', '\b', '\f', '\n', '\r', '\t'};

    size_t extra_length = 0;
    const char* p = s;
    while (*p) {
        for (size_t i = 0; i < sizeof(symbols_to_escape); ++i) {
            if (*p == symbols_to_escape[i]) {
                ++extra_length;
                break;
            }
        }
        ++p;
    }

    // original + extra + 1 for null terminator
    char* result = (char*)malloc(strlen(s) + extra_length + 1);
    if (result == NULL) {
        perror("Memory allocation failed");
        exit(1);
    }

    char* dst = result;
    for (p = s; *p; ++p) {
        char c = *p;
        switch (c) {
        case '\"':
            *dst++ = '\\';
            *dst++ = '\"';
            break;
        case '\\':
            *dst++ = '\\';
            *dst++ = '\\';
            break;
        case '\b':
            *dst++ = '\\';
            *dst++ = 'b';
            break;
        case '\f':
            *dst++ = '\\';
            *dst++ = 'f';
            break;
        case '\n':
            *dst++ = '\\';
            *dst++ = 'n';
            break;
        case '\r':
            *dst++ = '\\';
            *dst++ = 'r';
            break;
        case '\t':
            *dst++ = '\\';
            *dst++ = 't';
            break;
        default:
            *dst++ = c;
            break;
        }
    }

    *dst = '\0';
    return result;
}

void traverse_leaves(TSNode node, char* data) {
    uint32_t child_count = ts_node_child_count(node);
    if (child_count == 0) {
        uint32_t l = ts_node_start_byte(node), r = ts_node_end_byte(node);
        char* token = (char*)malloc(r - l + 1);
        strncpy(token, data + l, r - l);
        token[r - l] = '\0';
        char* encoded = encode_string(token);
        free(token);

        static bool is_first_token = true;
        if (is_first_token) {
            printf("[\n\t\"%s\"", encoded);
            is_first_token = false;
        } else {
            printf(",\n\t\"%s\"", encoded);
        }
        free(encoded);
    }

    for (uint32_t i = 0; i < child_count; ++i) {
        TSNode child = ts_node_child(node, i);
        traverse_leaves(child, data);
    }
}

const TSLanguage* choose_language(const char* filename) {
    const char* extension = strrchr(filename, '.');
    ++extension;
    if (strcmp(extension, "json") == 0) {
        return tree_sitter_json();
    }

    const char* cpp_extensions[] = {"cpp", "hpp", "cxx"};
    for (size_t i = 0; i < sizeof(cpp_extensions) / sizeof(cpp_extensions[0]);
         ++i) {
        if (strcmp(extension, cpp_extensions[i]) == 0) {
            return tree_sitter_cpp();
        }
    }

    const char* c_extensions[] = {"c", "h"};
    for (size_t i = 0; i < sizeof(c_extensions) / sizeof(c_extensions[0]);
         ++i) {
        if (strcmp(extension, c_extensions[i]) == 0) {
            return tree_sitter_c();
        }
    }

    if (strcmp(extension, "py") == 0) {
        return tree_sitter_python();
    }

    fprintf(stderr, "Unsupported file extension: %s\n", filename);
    exit(1);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input-file>\n", argv[0]);
        return 1;
    }

    TSParser* parser = ts_parser_new();
    ts_parser_set_language(parser, choose_language(argv[1]));

    char* data = read_file(argv[1]);
    TSTree* tree = ts_parser_parse_string(parser, NULL, data, strlen(data));

    TSNode root_node = ts_tree_root_node(tree);
    traverse_leaves(root_node, data);
    printf("\n]\n");

    free(data);
    ts_tree_delete(tree);
    ts_parser_delete(parser);
    return 0;
}
