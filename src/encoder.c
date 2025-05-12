#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include "encoder.h"

// ====================== FUNCTION SIGNATURE PARSER =====================

FunctionSignature parse_function_signature(const char *signature) {
    FunctionSignature result;
    memset(&result, 0, sizeof(FunctionSignature));

    const char *open_paren = strchr(signature, '(');
    const char *close_paren = strchr(signature, ')');

    if (!open_paren || !close_paren || close_paren < open_paren) {
        fprintf(stderr, "Invalid function signature format\n");
        exit(EXIT_FAILURE);
    }

    size_t name_len = open_paren - signature;
    strncpy(result.function_name, signature, name_len);
    result.function_name[name_len] = '\0';

    char params_buffer[256];
    size_t params_len = close_paren - open_paren - 1;
    strncpy(params_buffer, open_paren + 1, params_len);
    params_buffer[params_len] = '\0';

    char *token = strtok(params_buffer, ",");
    while (token && result.param_count < 10) {
        result.param_types[result.param_count] = strdup(token);
        result.param_count++;
        token = strtok(NULL, ",");
    }

    return result;
}

// ========================== ENCODING HELPERS ==========================

const char *strip_0x(const char *hex) {
    if (hex[0] == '0' && tolower(hex[1]) == 'x') {
        return hex + 2;
    }
    return hex;
}

char *encode_address(const char *value) {
    const char *clean = strip_0x(value);
    if (strlen(clean) != 40) {
        fprintf(stderr, "Invalid address length: %s\n", clean);
        exit(EXIT_FAILURE);
    }

    char *result = malloc(65); // 32 bytes = 64 chars + \0
    sprintf(result, "%064s", clean); // zero-padding on the left
    return result;
}

char *encode_uint256(const char *value) {
    char hex[65] = {0};
    unsigned long long int_val = strtoull(value, NULL, 10); // limited for now
    sprintf(hex, "%llx", int_val);

    char *result = malloc(65);
    sprintf(result, "%064s", hex); // zero-padding to 64 hex chars
    return result;
}
