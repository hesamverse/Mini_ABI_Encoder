#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cli.h"
#include "utils.h"

void show_help() {
    printf("Usage:\n");
    printf("  abi_encoder --signature \"<sig>\" --params \"<param1,param2,...>\" [options]\n");
    printf("\nOptions:\n");
    printf("  --only-selector       Print only 4-byte selector\n");
    printf("  --pretty              Print nicely formatted output\n");
    printf("  --json-out            Output as JSON\n");
    printf("  --json-file FILE      Load input from JSON file\n");
    printf("  -i, --interactive     Prompt for input interactively\n");
    printf("  -h, --help            Show this help message\n");
}

CLIInput parse_cli_args_full(int argc, char *argv[], int *only_selector, int *pretty, int *json_out, int *interactive) {
    CLIInput input = {0};

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--only-selector") == 0) {
            *only_selector = 1;
        } else if (strcmp(argv[i], "--pretty") == 0) {
            *pretty = 1;
        } else if (strcmp(argv[i], "--json-out") == 0) {
            *json_out = 1;
        } else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--interactive") == 0) {
            *interactive = 1;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            show_help();
            exit(0);
        } else if (strcmp(argv[i], "--signature") == 0 && i + 1 < argc) {
            strncpy(input.signature, argv[++i], sizeof(input.signature) - 1);
        } else if (strcmp(argv[i], "--params") == 0 && i + 1 < argc) {
            char *token = strtok(argv[++i], ",");
            while (token && input.param_count < 10) {
                input.params[input.param_count++] = my_strdup(token);
                token = strtok(NULL, ",");
            }
        }
    }

    return input;
}