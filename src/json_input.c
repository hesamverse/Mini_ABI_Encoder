#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "utils.h"
#include "encoder.h"
#include "json_input.h"

// Reads entire file into memory and returns a null-terminated string
static char *read_file_to_string(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Unable to open JSON input file");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    rewind(file);

    char *buffer = malloc(length + 1);
    if (!buffer) {
        fprintf(stderr, "Memory allocation failed for JSON buffer\n");
        exit(EXIT_FAILURE);
    }

    fread(buffer, 1, length, file);
    buffer[length] = '\0';
    fclose(file);
    return buffer;
}

// Parses the JSON and returns a populated CLIInput struct
CLIInput parse_json_file(const char *filename) {
    CLIInput input;
    memset(&input, 0, sizeof(CLIInput));

    char *json_text = read_file_to_string(filename);
    cJSON *root = cJSON_Parse(json_text);
    if (!root) {
        fprintf(stderr, "Error parsing JSON: %s\n", cJSON_GetErrorPtr());
        exit(EXIT_FAILURE);
    }

    // Parse "signature"
    cJSON *sig = cJSON_GetObjectItemCaseSensitive(root, "signature");
    if (!cJSON_IsString(sig) || (sig->valuestring == NULL)) {
        fprintf(stderr, "Invalid or missing 'signature' in JSON\n");
        exit(EXIT_FAILURE);
    }
    input.signature = my_strdup(sig->valuestring);

    // Parse "params"
    cJSON *params = cJSON_GetObjectItemCaseSensitive(root, "params");
    if (!cJSON_IsArray(params)) {
        fprintf(stderr, "Invalid or missing 'params' array in JSON\n");
        exit(EXIT_FAILURE);
    }

    int count = cJSON_GetArraySize(params);
    input.param_count = count;
    for (int i = 0; i < count; i++) {
        cJSON *item = cJSON_GetArrayItem(params, i);
        if (!cJSON_IsString(item)) {
            fprintf(stderr, "All 'params' must be strings\n");
            exit(EXIT_FAILURE);
        }
        input.params[i] = my_strdup(item->valuestring);
    }

    free(json_text);
    cJSON_Delete(root);
    return input;
}