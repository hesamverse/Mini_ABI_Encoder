// cli.h
// --------------------------------------------------
// Command-Line Argument Parser for Mini ABI Encoder
// --------------------------------------------------
// Author: Hesamverse
// Description:
//     Declares the functions used to parse CLI arguments
//     into usable data structures for encoding.
//
//     Depends on CLIInput struct defined in utils.h.
//
#ifndef CLI_H
#define CLI_H

#include "utils.h"  // Needed for CLIInput type

// Prints full help message to stdout
void print_help(void);

// Parses full CLI argument set into a CLIInput structure
CLIInput parse_cli_args_full(int argc, char *argv[], int *only_selector, int *pretty, int *json_out, int *interactive);

#endif // CLI_H
