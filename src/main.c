// main.c
// --------------------------------------------
// Entry Point for Mini ABI Encoder
// Supports CLI, JSON, and GUI-based input
// --------------------------------------------
// Author: Hesamverse
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "encoder.h"
#include "utils.h"
#include "keccak.h"
#include "json_input.h"
#include "cli.h"
#include "gui.h"
#include "interactive.h"

int main(int argc, char *argv[]) {
    // Check for GUI mode
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--gui") == 0) {
            for (int k = i; k < argc - 1; ++k)
                argv[k] = argv[k + 1];
            --argc;
            return launch_gui(argc, argv);
        }
    }

    // Help mode
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_help();
            return 0;
        }
    }

    // Interactive CLI mode
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--interactive") == 0 || strcmp(argv[i], "-i") == 0) {
            interactive_mode();
            return 0;
        }
    }

    // Default CLI parsing (non-interactive)
    int only_selector = 0, pretty = 0, json_out = 0, interactive = 0;
    CLIInput input = parse_cli_args_full(argc, argv, &only_selector, &pretty, &json_out, &interactive);
    
    if (input.signature[0] == '\0' || input.param_count == 0) {
        fprintf(stderr, "❌ Error: Missing --signature and/or --params.\nUse --help for usage.\n");
        return 1;
    }
    

    char *result = encode_input(&input);

    if (result) {
        printf("%s\n", result);
        free(result);
    }

    return 0;
}
