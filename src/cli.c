#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cli.h"
#include "utils.h"

void print_help() {
    printf("Mini ABI Encoder - Encode Solidity function calls\n\n");
    printf("Usage:\n");
    printf("  abi_encoder --signature \"func(type1,type2)\" --params \"val1,val2\" [OPTIONS]\n");
    printf("  abi_encoder --json-file <file.json>\n");
    printf("  abi_encoder --interactive | -i\n\n");
    printf("Options:\n");
    printf("  --only-selector        Output only the 4-byte function selector\n");
    printf("  --pretty               Output formatted calldata with labels\n");
    printf("  --json                 Output calldata in JSON format { selector, calldata }\n");
    printf("  --json-file <file>     Load signature and parameters from a JSON file\n");
    printf("  --interactive, -i      Prompt interactively for signature and params\n");
    printf("  --help, -h             Show this help message\n\n");
    printf("Examples:\n");
    printf("  abi_encoder --signature \"transfer(address,uint256)\" --params \"0xabc...,1000\"\n");
    printf("  abi_encoder --json-file tests/json/transfer.json --json\n");
    printf("  abi_encoder -i\n");
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