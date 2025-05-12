#ifndef KECCAK_H
#define KECCAK_H

#include <stdint.h>
#include <stddef.h>

void keccak256(const uint8_t *input, size_t input_len, uint8_t output[32]);

#endif