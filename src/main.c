#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "encoder.h"
#include "utils.h"
#include "keccak.h"
#include "json_input.h"


int main(int argc, char *argv[]) {
    CLIInput input;

    if (argc == 3 && strcmp(argv[1], "--json-file") == 0) {
        input = parse_json_file(argv[2]);
    } else {
        input = parse_cli_args(argc, argv);
    }
    
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

    // Arrays to hold encoded static and dynamic parameter values
    char *encoded_params[10];       // Static encoded values or offsets
    char *dynamic_data[10];         // Dynamic section data (e.g., for string/bytes)
    int dynamic_data_len[10];       // Lengths of each dynamic hex-encoded segment
    int dynamic_count = 0;
    int dynamic_offset = 32 * fsig.param_count;  // Offset in bytes

    int total_static_len = 8; // Initial length (8 hex chars for the selector)

    // Encode each parameter based on its type
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
            // Generate offset placeholder for dynamic string
            char offset_hex[65];
            sprintf(offset_hex, "%064x", dynamic_offset);
            encoded = my_strdup(offset_hex);

            int dyn_len = 0;
            char *dyn = encode_string(value, &dyn_len);
            dynamic_data[dynamic_count] = dyn;
            dynamic_data_len[dynamic_count] = dyn_len;
            dynamic_offset += dyn_len / 2; // Convert hex length to byte length
            dynamic_count++;
        } else if (strcmp(type, "bytes") == 0) {
            // Create offset hex value to be stored in static section
            char offset_hex[65];
            sprintf(offset_hex, "%064x", dynamic_offset);
            encoded = my_strdup(offset_hex);
        
            int dyn_len = 0;
            char *dyn = encode_bytes(value, &dyn_len);
            dynamic_data[dynamic_count] = dyn;
            dynamic_data_len[dynamic_count] = dyn_len;
            dynamic_offset += dyn_len / 2;  // Convert hex length to byte count
            dynamic_count++;
        } else {
            fprintf(stderr, "Unsupported type: %s\n", type);
            exit(EXIT_FAILURE);
        }

        encoded_params[i] = encoded;
        total_static_len += strlen(encoded);
    }

    // Add lengths of all dynamic segments
    for (int i = 0; i < dynamic_count; i++) {
        total_static_len += dynamic_data_len[i];
    }

    // Generate function selector using keccak256 of the signature string
    uint8_t hash[32];
    keccak256((uint8_t *)input.signature, strlen(input.signature), hash);

    char selector[9];
    for (int i = 0; i < 4; i++) {
        sprintf(selector + i * 2, "%02x", hash[i]);
    }
    selector[8] = '\0';

    // Allocate buffer for final calldata (selector + static + dynamic)
    char *calldata = malloc(total_static_len + 1);
    if (!calldata) {
        fprintf(stderr, "Memory allocation failed for calldata.\n");
        exit(EXIT_FAILURE);
    }
    calldata[0] = '\0';

    // Build the calldata: selector + encoded static values
    strcat(calldata, selector);
    for (int i = 0; i < fsig.param_count; i++) {
        printf("Appending param %d: %s\n", i + 1, encoded_params[i]);
        strcat(calldata, encoded_params[i]);
        free(encoded_params[i]);
    }

    // Append dynamic section at the end (string, bytes, etc.)
    for (int i = 0; i < dynamic_count; i++) {
        printf("Appending dynamic %d: %s\n", i + 1, dynamic_data[i]);
        strcat(calldata, dynamic_data[i]);
        free(dynamic_data[i]);
    }

    // Final output
    printf("\n💡 Final calldata:\n%s\n", calldata);
    free(calldata);

    return 0;
}