#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "utils.h"

// Simple strdup implementation
char *my_strdup(const char *s) {
    char *dup = malloc(strlen(s) + 1);
    if (dup) strcpy(dup, s);
    return dup;
}

CLIInput parse_cli_args(int argc, char *argv[]) {
    CLIInput input;
    memset(&input, 0, sizeof(CLIInput));

    bool sig_found = false;
    bool params_found = false;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--signature") == 0 && i + 1 < argc) {
            strncpy(input.signature, argv[i + 1], sizeof(input.signature) - 1);
            sig_found = true;
            i++;
        } else if (strcmp(argv[i], "--params") == 0 && i + 1 < argc) {
            char *params_raw = argv[i + 1];
            char *token = strtok(params_raw, ",");
            while (token && input.param_count < 10) {
                input.params[input.param_count] = my_strdup(token);
                input.param_count++;
                token = strtok(NULL, ",");
            }
            params_found = true;
            i++;
        }
    }

    if (!sig_found) {
        printf("Enter function signature (e.g. transfer(address,uint256)): ");
        fgets(input.signature, sizeof(input.signature), stdin);
        input.signature[strcspn(input.signature, "\n")] = '\0';
    }

    if (!params_found) {
        printf("Enter parameters separated by comma (e.g. 0xabc...,1000000): ");
        char buffer[512];
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        char *token = strtok(buffer, ",");
        while (token && input.param_count < 10) {
            input.params[input.param_count] = my_strdup(token);
            input.param_count++;
            token = strtok(NULL, ",");
        }
    }

    return input;
}