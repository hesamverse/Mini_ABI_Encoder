#ifndef ENCODER_H
#define ENCODER_H

typedef struct {
    char function_name[64];
    char *param_types[10];
    int param_count;
} FunctionSignature;

FunctionSignature parse_function_signature(const char *signature);
char *encode_address(const char *value);
char *encode_uint256(const char *value);
char *encode_bool(const char *value);

#endif