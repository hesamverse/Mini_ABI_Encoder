// interactive.c
// --------------------------------------------
// Interactive CLI Mode Implementation
// --------------------------------------------
// Author: Hesamverse
// Description:
//     Provides a menu-based interface for developers to
//     manually input Ethereum function signatures and parameters,
//     encode them via ABI rules, and print the results.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "encoder.h"
#include "utils.h"
#include "keccak.h"
#include "cli.h"

void interactive_mode(void) {
    char sig[256];
    char raw_params[512];

    printf("=== Mini ABI Encoder [Interactive Mode] ===\n");
    printf(">> This interface is for advanced developers.\n");
    printf(">> You will be prompted for function signature and parameters.\n");

    while (1) {
        // Step 1: Get signature
        printf("\n🔹 Enter function signature (e.g. transfer(address,uint256)): ");
        if (!fgets(sig, sizeof(sig), stdin)) break;
        sig[strcspn(sig, "\n")] = '\0';  // Strip newline

        // Step 2: Get parameters
        printf("🔹 Enter comma-separated parameters: ");
        if (!fgets(raw_params, sizeof(raw_params), stdin)) break;
        raw_params[strcspn(raw_params, "\n")] = '\0';

        // Step 3: Build CLIInput manually
        CLIInput input = {0};
        strncpy(input.signature, sig, sizeof(input.signature) - 1);
        char *token = strtok(raw_params, ",");
        while (token && input.param_count < 10) {
            input.params[input.param_count++] = my_strdup(token);
            token = strtok(NULL, ",");
        }

        // Step 4: Encode & output
        char *result = encode_input(&input);
        if (result) {
            printf("\n✅ ABI Encoded Output:\n%s\n", result);
            free(result);
        }

        // Step 5: Repeat or exit
        char choice[8];
        printf("\n🔁 Encode another? (y/n): ");
        if (!fgets(choice, sizeof(choice), stdin)) break;
        if (choice[0] != 'y' && choice[0] != 'Y') break;
    }

    printf("\n👋 Exiting interactive mode.\n");
}
