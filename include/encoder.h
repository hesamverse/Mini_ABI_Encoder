// encoder.h
// ------------------------------------------------------
// ABI Encoding Functions and Signature Parsing
// ------------------------------------------------------
// Author: Hesamverse
// Description:
//     Provides function declarations for ABI encoding,
//     including support for address, uint256, string, bytes, and bool types.
//     Also includes function selector generation and ABI serialization.
//
//     NOTE: CLIInput is defined in utils.h and must not be redefined here.
//
#ifndef ENCODER_H
#define ENCODER_H

#include "utils.h"  // For CLIInput
#include "cli.h"    // Optional, if CLI flags are used in encoder (can be removed if not needed)

// Represents the parsed function name and parameter types (parsed from signature)
typedef struct {
    char function_name[64];     // e.g., "transfer"
    char *param_types[10];      // e.g., {"address", "uint256"}
    int param_count;            // number of parameter types
} FunctionSignature;

// Parses a Solidity-style function signature and extracts name and types
FunctionSignature parse_function_signature(const char *signature);

// ABI encoding functions for each supported Solidity type
char *encode_address(const char *value);                       // Encodes Ethereum address
char *encode_uint256(const char *value);                       // Encodes uint256 number
char *encode_bool(const char *value);                          // Encodes boolean (true/false)
char *encode_string(const char *value, int *out_len);          // Encodes dynamic string with length prefix
char *encode_bytes(const char *hex_data, int *out_len);        // Encodes dynamic bytes (0x-prefixed hex)

// Main function that receives parsed CLI input and returns ABI-encoded output
char *encode_input(const CLIInput *input);

#endif // ENCODER_H
