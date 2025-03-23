#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const char* dt_ext[] = {"c",  "h",  "cpp", "hpp", "go",
                        "rs", "py", "js",  "ts",  "json"};

const char* lsp_ext[] = {"lsp"}; // There is no lsp team

int is_in_array(const char* str, const char** array, int num) {
    for (int i = 0; i < num; ++i) {
        if (strcmp(str, array[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

const char* choose_implementation(const char* filename) {
    const char* dot = strrchr(filename, '.');
    // If no extension
    if (dot == NULL || dot == filename) {
        return "./fork-in-regex";
    }
    const char* ext = dot + 1; // without dot

    // Check dt
    if (is_in_array(ext, dt_ext, sizeof(dt_ext) / sizeof(dt_ext[0]))) {
        return "./fork-in-dt";
    }

    // Check lsp
    if (is_in_array(ext, lsp_ext, sizeof(lsp_ext) / sizeof(lsp_ext[0]))) {
        return "./fork-in-lsp";
    }

    // Default to regex
    return "./fork-in-regex";
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char* impl = choose_implementation(argv[1]);
    // Should not return
    execlp(impl, impl, argv[1], NULL);

    perror("execlp failed");
    exit(EXIT_FAILURE);
}
