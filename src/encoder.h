#ifndef ENCODER_H
#define ENCODER_H

typedef struct {
    char function_name[64];
    char *param_types[10];
    int param_count;
} FunctionSignature;

FunctionSignature parse_function_signature(const char *signature);

#endif