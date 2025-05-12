#ifndef JSON_INPUT_H
#define JSON_INPUT_H

#include "encoder.h"  // For CLIInput struct

// Parses a JSON file and returns a CLIInput struct containing the function signature and parameters.
// Expected JSON format:
// {
//     "signature": "transfer(address,uint256)",
//     "params": ["0xabc123...", "1000000000000000000"]
// }
CLIInput parse_json_file(const char *filename);

#endif