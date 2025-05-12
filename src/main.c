#include <stdio.h>
#include "encoder.h"

int main() {
    const char *signature = "transfer(address,uint256)";
    FunctionSignature parsed = parse_function_signature(signature);

    printf("Function name: %s\n", parsed.function_name);
    printf("Param count: %d\n", parsed.param_count);
    for (int i = 0; i < parsed.param_count; i++) {
        printf("Param %d: %s\n", i + 1, parsed.param_types[i]);
    }

    return 0;
}