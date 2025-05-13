#ifndef UTILS_H
#define UTILS_H
#include "encoder.h"

// Type definition for CLI input structure
typedef struct {
    char signature[256];
    char *params[10];
    int param_count;
} CLIInput;

// Custom strdup wrapper
char *my_strdup(const char *s); // Declaration only

#endif