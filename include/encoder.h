#ifndef ENCODER_H
#define ENCODER_H

// Represents the parsed function signature and parameter types
typedef struct {
    char function_name[64];   // e.g., "transfer"
    char *param_types[10];    // e.g., {"address", "uint256"}
    int param_count;          // number of parameters
} FunctionSignature;

// Used across input systems (CLI, JSON, etc.)
typedef struct {
    char signature[128];      // Full function signature, e.g., "transfer(address,uint256)"
    char *params[10];         // Parameter values as strings
    int param_count;          // Number of values provided
} CLIInput;

// Parses a Solidity-style function signature and extracts name and types
FunctionSignature parse_function_signature(const char *signature);

// ABI encoding functions for each supported type
char *encode_address(const char *value);                       // Encodes Ethereum address
char *encode_uint256(const char *value);                       // Encodes uint256 number
char *encode_bool(const char *value);                          // Encodes boolean (true/false)
char *encode_string(const char *value, int *out_len);          // Encodes dynamic string with length prefix
char *encode_bytes(const char *hex_data, int *out_len);        // Encodes dynamic bytes (0x-prefixed hex)

#endif // ENCODER_H
