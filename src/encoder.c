#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include "encoder.h"
#include "utils.h"

// Pads a hex string with leading zeros to fit the specified width (typically 64 for 32 bytes)
void left_pad_hex(char *dest, const char *src, int width) {
    int len = strlen(src);
    int pad = width - len;
    memset(dest, '0', pad);
    strcpy(dest + pad, src);
    dest[width] = '\0';
}

// ========================== FUNCTION SIGNATURE PARSER ==========================

// Parses a Solidity-style function signature, e.g., "transfer(address,uint256)"
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
        result.param_types[result.param_count] = my_strdup(token);
        result.param_count++;
        token = strtok(NULL, ",");
    }

    return result;
}

// ========================== ENCODING HELPERS ==========================

// Removes '0x' prefix from hex input if present
const char *strip_0x(const char *hex) {
    if (hex[0] == '0' && tolower(hex[1]) == 'x') {
        return hex + 2;
    }
    return hex;
}

// Encodes an Ethereum address (20 bytes) into 32-byte hex (left-padded with zeros)
char *encode_address(const char *value) {
    const char *clean = strip_0x(value);
    if (strlen(clean) != 40) {
        fprintf(stderr, "Invalid address length: %s\n", clean);
        exit(EXIT_FAILURE);
    }

    char *result = malloc(65); // 64 hex chars + null terminator
    if (!result) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    left_pad_hex(result, clean, 64);
    return result;
}

// Encodes a uint256 value into a 32-byte hex string
char *encode_uint256(const char *value) {
    char hex[65] = {0};
    unsigned long long int_val = strtoull(value, NULL, 10);
    sprintf(hex, "%llx", int_val);

    char *result = malloc(65);
    if (!result) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    left_pad_hex(result, hex, 64);
    return result;
}

// Encodes a boolean value ("true"/"false", "1"/"0") into 32-byte hex string
char *encode_bool(const char *value) {
    char *result = malloc(65); // 64 hex chars + null terminator
    if (!result) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp(value, "true") == 0 || strcmp(value, "1") == 0) {
        sprintf(result, "%064x", 1);
    } else if (strcmp(value, "false") == 0 || strcmp(value, "0") == 0) {
        sprintf(result, "%064x", 0);
    } else {
        fprintf(stderr, "Invalid boolean value: %s\n", value);
        free(result);
        exit(EXIT_FAILURE);
    }

    return result;
}

// Encodes a string as a dynamic type in ABI format
// Output: [32 bytes length][string data (padded to 32 bytes)]
// The returned string is a hex-encoded version of both the length and padded data
char *encode_string(const char *value, int *out_len) {
    size_t len = strlen(value);
    size_t padded_len = ((len + 31) / 32) * 32;
    *out_len = 64 + padded_len * 2; // 64 hex chars for length + padded data

    char *result = malloc(*out_len + 1); // +1 for null terminator
    if (!result) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    // Encode the string length in hex (32 bytes left-padded)
    sprintf(result, "%064lx", len);

    // Encode each character into 2-digit hex, pad remaining with "00"
    for (size_t i = 0; i < padded_len; i++) {
        char byte = (i < len) ? value[i] : 0;
        sprintf(result + 64 + i * 2, "%02x", (unsigned char)byte);
    }

    result[*out_len] = '\0';
    return result;
}