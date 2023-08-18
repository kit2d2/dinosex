#pragma once

#include <stdint.h>
// a normal binary encoder

struct rr_binary_encoder
{
    uint8_t *at;
    uint8_t *start;
};

void rr_binary_encoder_init(struct rr_binary_encoder *, uint8_t *);
uint8_t rr_binary_encoder_read_uint8(struct rr_binary_encoder *);
uint64_t rr_binary_encoder_read_varuint(struct rr_binary_encoder *);
float rr_binary_encoder_read_float32(struct rr_binary_encoder *);
double rr_binary_encoder_read_float64(struct rr_binary_encoder *);
void rr_binary_encoder_read_nt_string(struct rr_binary_encoder *, char *);