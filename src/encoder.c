// encoder.c
// ------------------------------------------------------------
// ABI Encoding Core Logic
// ------------------------------------------------------------
// Author: Hesamverse
// Description:
//     Contains core functions for parsing Solidity function
//     signatures and encoding arguments according to Ethereum ABI.
//     Supports types: address, uint256, bool, string, bytes
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#include "encoder.h"
#include "utils.h"
#include "keccak.h"

// Pads a hex string with leading zeros to fit the specified width (typically 64 for 32 bytes)
void left_pad_hex(char *dest, const char *src, int width) {
    int len = strlen(src);
    int pad = width - len;
    memset(dest, '0', pad);
    strcpy(dest + pad, src);
    dest[width] = '\0';
}

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

// Strips 0x prefix from hex string
const char *strip_0x(const char *hex) {
    if (hex[0] == '0' && tolower(hex[1]) == 'x') {
        return hex + 2;
    }
    return hex;
}

// Encodes address into 32-byte hex string
char *encode_address(const char *value) {
    const char *clean = strip_0x(value);
    if (strlen(clean) != 40) {
        fprintf(stderr, "Invalid address length: %s\n", clean);
        exit(EXIT_FAILURE);
    }

    char *result = malloc(65);
    left_pad_hex(result, clean, 64);
    return result;
}

// Encodes uint256 into 32-byte hex
char *encode_uint256(const char *value) {
    char hex[65] = {0};
    unsigned long long int_val = strtoull(value, NULL, 10);
    sprintf(hex, "%llx", int_val);

    char *result = malloc(65);
    left_pad_hex(result, hex, 64);
    return result;
}

// Encodes boolean into 32-byte hex
char *encode_bool(const char *value) {
    char *result = malloc(65);
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

// Encodes string as [length + data] format
char *encode_string(const char *value, int *out_len) {
    size_t len = strlen(value);
    size_t padded_len = ((len + 31) / 32) * 32;
    *out_len = 64 + padded_len * 2;

    char *result = malloc(*out_len + 1);
    sprintf(result, "%064lx", len);

    for (size_t i = 0; i < padded_len; i++) {
        char byte = (i < len) ? value[i] : 0;
        sprintf(result + 64 + i * 2, "%02x", (unsigned char)byte);
    }

    result[*out_len] = '\0';
    return result;
}

// Encodes bytes (0x-prefixed hex) into ABI format
char *encode_bytes(const char *hex_data, int *out_len) {
    const char *clean = strip_0x(hex_data);
    size_t hex_len = strlen(clean);

    if (hex_len % 2 != 0) {
        fprintf(stderr, "Invalid bytes: odd length\n");
        exit(EXIT_FAILURE);
    }

    size_t byte_len = hex_len / 2;
    size_t padded_bytes = ((byte_len + 31) / 32) * 32;

    *out_len = 64 + padded_bytes * 2;
    char *result = malloc(*out_len + 1);
    sprintf(result, "%064lx", byte_len);

    for (size_t i = 0; i < padded_bytes; i++) {
        if (i < byte_len) {
            result[64 + i * 2] = clean[i * 2];
            result[64 + i * 2 + 1] = clean[i * 2 + 1];
        } else {
            result[64 + i * 2] = '0';
            result[64 + i * 2 + 1] = '0';
        }
    }

    result[*out_len] = '\0';
    return result;
}

// Main encoder: receives CLIInput and returns ABI-encoded calldata
char *encode_input(const CLIInput *input) {
    FunctionSignature sig = parse_function_signature(input->signature);

    if (sig.param_count != input->param_count) {
        fprintf(stderr, "Parameter count mismatch\n");
        exit(EXIT_FAILURE);
    }

    // Compute 4-byte selector
    char full_sig[256];
    snprintf(full_sig, sizeof(full_sig), "%s(", sig.function_name);
    for (int i = 0; i < sig.param_count; i++) {
        strcat(full_sig, sig.param_types[i]);
        if (i < sig.param_count - 1)
            strcat(full_sig, ",");
    }
    strcat(full_sig, ")");

    uint8_t hash[32];
    keccak256((const uint8_t *)full_sig, strlen(full_sig), hash);

    char selector[9];
    for (int i = 0; i < 4; i++)
        sprintf(selector + i * 2, "%02x", hash[i]);
    selector[8] = '\0';

    // Encode params
    char *encoded_parts[10] = {0};
    int total_len = 0;

    for (int i = 0; i < sig.param_count; i++) {
        const char *type = sig.param_types[i];
        const char *value = input->params[i];
        char *encoded = NULL;
        int len = 0;

        if (strcmp(type, "address") == 0) {
            encoded = encode_address(value);
            len = 64;
        } else if (strcmp(type, "uint256") == 0 || strcmp(type, "uint") == 0) {
            encoded = encode_uint256(value);
            len = 64;
        } else if (strcmp(type, "bool") == 0) {
            encoded = encode_bool(value);
            len = 64;
        } else if (strcmp(type, "string") == 0) {
            encoded = encode_string(value, &len);
        } else if (strcmp(type, "bytes") == 0) {
            encoded = encode_bytes(value, &len);
        } else {
            fprintf(stderr, "Unsupported type: %s\n", type);
            exit(EXIT_FAILURE);
        }

        encoded_parts[i] = encoded;
        total_len += len;
    }

    int final_len = 8 + total_len;
    char *calldata = malloc(final_len + 1);
    strcpy(calldata, selector);

    for (int i = 0; i < sig.param_count; i++) {
        strcat(calldata, encoded_parts[i]);
        free(encoded_parts[i]);
    }

    calldata[final_len] = '\0';
    return calldata;
}