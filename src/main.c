// src/main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "encoder.h"
#include "utils.h"
#include "keccak.h"
#include "json_input.h"
#include "cli.h"
#include "gui.h"

/*
 * Entry point for Mini ABI Encoder
 * Supports both CLI and GUI interfaces
 */
int main(int argc, char *argv[]) {
    // GUI mode launcher
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--gui") == 0) {
            // Shift argv left to remove --gui before passing to GTK
            for (int k = i; k < argc - 1; ++k)
                argv[k] = argv[k + 1];
            --argc;
            return launch_gui(argc, argv);
        }
    }

    // Help flag
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_help();
            return 0;
        }
    }

    // TODO: CLI input parsing and encoding logic
    CLIInput input = parse_cli_args_full(argc, argv);
    char *result = encode_input(&input);
    puts(result);
    free(result);

    return 0;
}