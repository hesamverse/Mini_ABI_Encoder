#ifndef UTILS_H
#define UTILS_H

typedef struct {
    char signature[128];
    char *params[10];
    int param_count;
} CLIInput;

CLIInput parse_cli_args(int argc, char *argv[]);

char *my_strdup(const char *s);  // Declaration only

#endif