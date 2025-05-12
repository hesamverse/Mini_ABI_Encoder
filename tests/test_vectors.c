#include <stdio.h>
#include <stdlib.h>

int main() {
    int result = system("./abi_encoder --json-file tests/json/transfer.json");
    if (result != 0) {
        fprintf(stderr, "Test failed: transfer.json\n");
        return 1;
    }
    printf("Test passed: transfer.json\n");
    return 0;
}