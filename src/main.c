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
 *   - GUI mode for beginner users
 */
int main(int argc, char *argv[]) {
    int only_selector = 0;
    int pretty = 0;
    int json_out = 0;
    int interactive = 0;

    // ✅ If --gui is present in any argument, launch GTK GUI
    for (int k = i; k < argc - 1; k++) argv[k] = argv[k + 1];
    argc--;
    return launch_gui(argc, argv);
    

    // ✅ Help flag
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_help();
            return 0;
        }
    }

    CLIInput input = {0};

    // ✅ Input from JSON file
    if (argc == 3 && strcmp(argv[1], "--json-file") == 0) {
        input = parse_json_file(argv[2]);
    }
    // ✅ Interactive prompt
    else if (argc == 1 || (argc >= 2 && 
            (strcmp(argv[1], "-i") == 0 || strcmp(argv[1], "--interactive") == 0))) {
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
    // ✅ CLI flags input
    else {
        input = parse_cli_args_full(argc, argv, &only_selector, &pretty, &json_out, &interactive);
    }

    // ✅ Parse function signature and extract function name + param types
    FunctionSignature fsig = parse_function_signature(input.signature);

    // ✅ Generate Keccak-256 function selector (first 4 bytes)
    uint8_t hash[32];
    keccak256((uint8_t *)input.signature, strlen(input.signature), hash);

    char selector[9];
    for (int i = 0; i < 4; i++) {
        sprintf(selector + i * 2, "%02x", hash[i]);
    }
    selector[8] = '\0';

    // ✅ If only selector is requested, print and exit
    if (only_selector) {
        printf("%s\n", selector);
        return 0;
    }

    // ✅ Encode each parameter
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

    // ✅ Allocate buffer and build calldata: selector + static + dynamic
    char *calldata = malloc(total_static_len + 1);
    if (!calldata) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    calldata[0] = '\0';
    strcat(calldata, selector);

    for (int i = 0; i < fsig.param_count; i++) {
        strcat(calldata, encoded_params[i]);
        free(encoded_params[i]);
    }

    for (int i = 0; i < dynamic_count; i++) {
        strcat(calldata, dynamic_data[i]);
        free(dynamic_data[i]);
    }

    // ✅ Output
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
