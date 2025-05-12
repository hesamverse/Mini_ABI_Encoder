#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "encoder.h"
#include "utils.h"
#include "keccak.h"
#include "json_input.h"
#include "cli.h"
#include "gui.h"

/**
 * Entry point of the Mini ABI Encoder CLI tool.
 * Supports:
 *   - Direct CLI input via flags
 *   - Interactive user prompts
 *   - JSON-based input for automation
 *   - Optional output formats: raw, pretty, JSON, selector-only
 */
int main(int argc, char *argv[]) {
    int only_selector = 0;  // Flag: output only the 4-byte selector
    int pretty = 0;         // Flag: output human-readable format
    int json_out = 0;       // Flag: output as JSON object
    int interactive = 0;    // Flag: enable stdin prompt

    CLIInput input;

    if (argc == 2 && strcmp(argv[1], "--gui") == 0) {
        return launch_gui(argc, argv);
    }
    
    // Help flag handling
    if (argc >= 2 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
        print_help();
        return 0;
    }

    // Input mode: JSON file mode
    if (argc == 3 && strcmp(argv[1], "--json-file") == 0) {
        input = parse_json_file(argv[2]);
    }
    // Input mode: Interactive prompt
    else if (argc == 1 || (argc >= 2 && (strcmp(argv[1], "-i") == 0 || strcmp(argv[1], "--interactive") == 0))) {
        interactive = 1;
        printf("Enter function signature (e.g. transfer(address,uint256)): ");
        fgets(input.signature, sizeof(input.signature), stdin);
        input.signature[strcspn(input.signature, "\n")] = 0;

        printf("Enter parameters separated by commas: ");
        char line[512];
        fgets(line, sizeof(line), stdin);
        line[strcspn(line, "\n")] = 0;

        char *token = strtok(line, ",");
        while (token && input.param_count < 10) {
            input.params[input.param_count++] = my_strdup(token);
            token = strtok(NULL, ",");
        }
    }
    // Input mode: CLI flags
    else {
        input = parse_cli_args_full(argc, argv, &only_selector, &pretty, &json_out, &interactive);
    }

    // Parse function signature into name and param types
    FunctionSignature fsig = parse_function_signature(input.signature);

    // Generate 4-byte function selector using Keccak256 hash
    uint8_t hash[32];
    keccak256((uint8_t *)input.signature, strlen(input.signature), hash);

    char selector[9];
    for (int i = 0; i < 4; i++) {
        sprintf(selector + i * 2, "%02x", hash[i]);
    }
    selector[8] = '\0';

    // If --only-selector was passed, print and exit
    if (only_selector) {
        printf("%s\n", selector);
        return 0;
    }

    // Arrays for encoded static and dynamic parameters
    char *encoded_params[10];       // Encoded static values or offset placeholders
    char *dynamic_data[10];         // Encoded dynamic values (e.g., string, bytes)
    int dynamic_data_len[10];       // Length of each dynamic value in hex
    int dynamic_count = 0;

    int dynamic_offset = 32 * fsig.param_count;  // Offset (in bytes) from start of calldata
    int total_static_len = 8; // Initial length = selector (8 hex chars)

    // Encode each parameter
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
            // Store dynamic offset in static section
            char offset_hex[65];
            sprintf(offset_hex, "%064x", dynamic_offset);
            encoded = my_strdup(offset_hex);

            int dyn_len = 0;
            char *dyn = encode_string(value, &dyn_len);
            dynamic_data[dynamic_count] = dyn;
            dynamic_data_len[dynamic_count] = dyn_len;
            dynamic_offset += dyn_len / 2;  // Count in bytes
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

    // Add dynamic section size to total
    for (int i = 0; i < dynamic_count; i++) {
        total_static_len += dynamic_data_len[i];
    }

    // Allocate space for full calldata (selector + static + dynamic)
    char *calldata = malloc(total_static_len + 1);
    if (!calldata) {
        fprintf(stderr, "Memory allocation failed for calldata.\n");
        exit(EXIT_FAILURE);
    }

    calldata[0] = '\0';
    strcat(calldata, selector);

    // Append encoded static parameters
    for (int i = 0; i < fsig.param_count; i++) {
        strcat(calldata, encoded_params[i]);
        free(encoded_params[i]);
    }

    // Append dynamic data at the end
    for (int i = 0; i < dynamic_count; i++) {
        strcat(calldata, dynamic_data[i]);
        free(dynamic_data[i]);
    }

    // Output formatting
    if (json_out) {
        printf("{\n  \"selector\": \"%s\",\n  \"calldata\": \"%s\"\n}\n", selector, calldata);
    } else if (pretty) {
        printf("\n📌 Function Selector: %s\n", selector);
        printf("📦 ABI-encoded calldata:\n%s\n", calldata);
    } else {
        printf("%s\n", calldata);
    }

    free(calldata);
    return 0;
}