#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "encoder.h"
#include "utils.h"
#include "keccak.h"
#include "json_input.h"

int main(int argc, char *argv[]) {
    CLIInput input;
    bool only_selector = false;
    bool pretty = false;
    bool json_out = false;

    // Detect extra flags
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--only-selector") == 0) only_selector = true;
        if (strcmp(argv[i], "--pretty") == 0) pretty = true;
        if (strcmp(argv[i], "--json-out") == 0) json_out = true;
    }

    // Select input mode
    if (argc >= 3 && strcmp(argv[1], "--json-file") == 0) {
        input = parse_json_file(argv[2]);
    } else {
        input = parse_cli_args(argc, argv);
    }

    FunctionSignature fsig = parse_function_signature(input.signature);

    // Generate function selector
    uint8_t hash[32];
    keccak256((uint8_t *)input.signature, strlen(input.signature), hash);
    char selector[9];
    for (int i = 0; i < 4; i++) {
        sprintf(selector + i * 2, "%02x", hash[i]);
    }
    selector[8] = '\0';

    if (only_selector) {
        printf("%s\n", selector);
        return 0;
    }

    if (pretty) {
        printf("\nFunction: %s\n", fsig.function_name);
        printf("Types   :");
        for (int i = 0; i < fsig.param_count; i++) printf(" %s", fsig.param_types[i]);
        printf("\nValues  :");
        for (int i = 0; i < input.param_count; i++) printf(" %s", input.params[i]);
        printf("\n");
    }

    char *encoded_params[10];
    char *dynamic_data[10];
    int dynamic_data_len[10];
    int dynamic_count = 0;
    int dynamic_offset = 32 * fsig.param_count;
    int total_static_len = 8;

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
        } else if (strcmp(type, "string") == 0) {
            char offset_hex[65];
            sprintf(offset_hex, "%064x", dynamic_offset);
            encoded = my_strdup(offset_hex);
            int dyn_len = 0;
            char *dyn = encode_string(value, &dyn_len);
            dynamic_data[dynamic_count] = dyn;
            dynamic_data_len[dynamic_count] = dyn_len;
            dynamic_offset += dyn_len / 2;
            dynamic_count++;
        } else if (strcmp(type, "bytes") == 0) {
            char offset_hex[65];
            sprintf(offset_hex, "%064x", dynamic_offset);
            encoded = my_strdup(offset_hex);
            int dyn_len = 0;
            char *dyn = encode_bytes(value, &dyn_len);
            dynamic_data[dynamic_count] = dyn;
            dynamic_data_len[dynamic_count] = dyn_len;
            dynamic_offset += dyn_len / 2;
            dynamic_count++;
        } else {
            fprintf(stderr, "Unsupported type: %s\n", type);
            exit(EXIT_FAILURE);
        }

        encoded_params[i] = encoded;
        total_static_len += strlen(encoded);
    }

    for (int i = 0; i < dynamic_count; i++) {
        total_static_len += dynamic_data_len[i];
    }

    char *calldata = malloc(total_static_len + 1);
    if (!calldata) {
        fprintf(stderr, "Memory allocation failed for calldata.\n");
        exit(EXIT_FAILURE);
    }
    calldata[0] = '\0';

    strcat(calldata, selector);
    for (int i = 0; i < fsig.param_count; i++) {
        if (pretty) printf("Appending param %d: %s\n", i + 1, encoded_params[i]);
        strcat(calldata, encoded_params[i]);
        free(encoded_params[i]);
    }

    for (int i = 0; i < dynamic_count; i++) {
        if (pretty) printf("Appending dynamic %d: %s\n", i + 1, dynamic_data[i]);
        strcat(calldata, dynamic_data[i]);
        free(dynamic_data[i]);
    }

    if (json_out) {
        printf("{\n  \"function\": \"%s\",\n  \"selector\": \"%s\",\n  \"calldata\": \"%s\"\n}\n",
            fsig.function_name, selector, calldata);
    } else {
        printf("\n\U0001F4A1 Final calldata:\n%s\n", calldata);
    }

    free(calldata);
    return 0;
}