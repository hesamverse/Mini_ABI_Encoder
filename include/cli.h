#ifndef CLI_H
#define CLI_H

#include "encoder.h"

CLIInput parse_cli_args_full(int argc, char *argv[], int *only_selector, int *pretty, int *json_out, int *interactive);
void show_help();

#endif // CLI_H