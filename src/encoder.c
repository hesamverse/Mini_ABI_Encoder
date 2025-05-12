#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "encoder.h"

FunctionSignature parse_function_signature(const char *signature) {
    FunctionSignature result;
    memset(&result, 0, sizeof(FunctionSignature)); // Zero the structure

    // Finding the position of open and close parentheses
    const char *open_paren = strchr(signature, '(');
    const char *close_paren = strchr(signature, ')');

    if (!open_paren || !close_paren || close_paren < open_paren) {
        fprintf(stderr, "Invalid function signature format\n");
        exit(EXIT_FAILURE);
    }

    // Extracting name from function
    size_t name_len = open_paren - signature;
    strncpy(result.function_name, signature, name_len);
    result.function_name[name_len] = '\0';

    // Parameter extraction
    char params_buffer[256];
    size_t params_len = close_paren - open_paren - 1;
    strncpy(params_buffer, open_paren + 1, params_len);
    params_buffer[params_len] = '\0';

    // Separating parameters by comma
    char *token = strtok(params_buffer, ",");
    while (token && result.param_count < 10) {
        result.param_types[result.param_count] = strdup(token);
        result.param_count++;
        token = strtok(NULL, ",");
    }

    return result;
}