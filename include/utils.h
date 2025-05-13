// utils.h
// --------------------------------------------------------
// Utility Functions and Core Data Structures for ABI Encoder
// --------------------------------------------------------
// Author: Hesamverse
// Description:
//     Defines shared helper functions and the CLIInput struct,
//     used for command-line parsing and parameter storage.
//     This header must be included by any module that interacts
//     with ABI encoding or input parsing.
//

#ifndef UTILS_H
#define UTILS_H

// Structure to hold user-supplied function signature and parameters
typedef struct {
    char signature[256];     // Solidity-style function signature, e.g., transfer(address,uint256)
    char *params[10];        // Parameter values, max 10 inputs
    int param_count;         // Number of input parameters
} CLIInput;

// Dynamically duplicates a string (like strdup, portable)
char *my_strdup(const char *s);

#endif // UTILS_H
