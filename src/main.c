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

    // Encode all parameters to their ABI format
    char *encoded_params[10];
    int total_len = 8; // Start with 8 hex characters for the function selector

    for (int i = 0; i < fsig.param_count; i++) {
        const char *type = fsig.param_types[i];
        const char *value = input.params[i];
        char *encoded = NULL;

        if (strcmp(type, "address") == 0) {
            encoded = encode_address(value);
        } else if (strcmp(type, "uint256") == 0) {
            encoded = encode_uint256(value);
        } else if (strcmp(type, "bool") == 0) {
            encoded = encode_bool(value);
        } else {
            fprintf(stderr, "Unsupported type: %s\n", type);
            exit(EXIT_FAILURE);
        }

        encoded_params[i] = encoded;
        total_len += strlen(encoded);
    }

    // Generate function selector using keccak256(function_signature)
    uint8_t hash[32];
    keccak256((uint8_t *)input.signature, strlen(input.signature), hash);

    char selector[9];
    for (int i = 0; i < 4; i++) {
        sprintf(selector + i * 2, "%02x", hash[i]);
    }
    selector[8] = '\0';

    // Build final ABI-encoded calldata: selector + encoded parameters
    char *calldata = malloc(total_len + 1); // +1 for null terminator
    if (!calldata) {
        fprintf(stderr, "Memory allocation failed for calldata.\n");
        exit(EXIT_FAILURE);
    }

    calldata[0] = '\0';
    strcat(calldata, selector);

    for (int i = 0; i < fsig.param_count; i++) {
        printf("Appending param %d: %s\n", i + 1, encoded_params[i]);
        strcat(calldata, encoded_params[i]);
        free(encoded_params[i]); // Free memory after use
    }

    printf("\n💡 Final calldata:\n%s\n", calldata);
    free(calldata);

    return 0;
}