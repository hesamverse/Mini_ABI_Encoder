#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "encoder.h"
#include "utils.h"
#include "keccak.h"

int main(int argc, char *argv[]) {
    CLIInput input = parse_cli_args(argc, argv);
    FunctionSignature fsig = parse_function_signature(input.signature);

    printf("\nFunction: %s\n", fsig.function_name);
    printf("Types   :");
    for (int i = 0; i < fsig.param_count; i++) {
        printf(" %s", fsig.param_types[i]);
    }
    printf("\nValues  :");
    for (int i = 0; i < input.param_count; i++) {
        printf(" %s", input.params[i]);
    }
    printf("\n");

    // Step 1: Encode all params
    char *encoded_params[10];
    int total_len = 0;

    for (int i = 0; i < fsig.param_count; i++) {
        const char *type = fsig.param_types[i];
        const char *value = input.params[i];
        char *encoded = NULL;

        if (strcmp(type, "address") == 0) {
            encoded = encode_address(value);
        } else if (strcmp(type, "uint256") == 0) {
            encoded = encode_uint256(value);
        } else {
            fprintf(stderr, "Unsupported type: %s\n", type);
            exit(EXIT_FAILURE);
        }

        encoded_params[i] = encoded;
        total_len += strlen(encoded);
    }

    // Step 2: Keccak256(function_signature) → selector
    uint8_t hash[32];
    keccak256((uint8_t *)input.signature, strlen(input.signature), hash);

    char selector[9];
    for (int i = 0; i < 4; i++) {
        sprintf(selector + i * 2, "%02x", hash[i]);
    }
    selector[8] = '\0';

    // Step 3: Build final calldata
    char *calldata = malloc(total_len + 9); // 8 for selector + 1 for '\0'
    strcpy(calldata, selector);
    for (int i = 0; i < fsig.param_count; i++) {
        printf("Appending param %d: %s\n", i + 1, encoded_params[i]);
        strcat(calldata, encoded_params[i]);
        free(encoded_params[i]); // Free after use
    }

    printf("\n💡 Final calldata:\n%s\n", calldata);
    free(calldata);

    return 0;
}