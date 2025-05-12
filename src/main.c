#include <stdio.h>
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

    return 0;
}