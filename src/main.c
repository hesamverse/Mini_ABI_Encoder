#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "encoder.h"
#include "utils.h"

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

        printf("Encoded param %d (%s): %s\n", i + 1, type, encoded);
        free(encoded);
    }

    return 0;
}