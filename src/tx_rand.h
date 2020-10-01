#pragma once

#include "tx_types.h"

void txrng_seed(uint32_t seed);
double txrng_next();
int32_t txrng_range(int32_t min, int32_t max);
float txrng_rangef(float min, float max);

// implementation
#if defined(TX_RAND_IMPLEMENTATION)

uint32_t _txrng_xorshift128_next(uint32_t state[4])
{
    uint32_t t = state[3];
    t ^= t << 11;
    t ^= t >> 8;

    state[3] = state[2];
    state[2] = state[1];
    state[1] = state[0];

    const uint32_t s = state[0];
    state[0] = t ^ s ^ (s >> 19);

    return state[0];
}

void _txrng_seed(uint32_t state[4], uint32_t seed)
{
    state[0] = seed;
    state[1] = seed ^ 0xAAAAAAAA;
    state[2] = seed ^ 0x55555555;
    state[3] = state[1] ^ state[2];
}

uint32_t default_rng[4];

void txrng_seed(uint32_t seed)
{
    _txrng_seed(default_rng, seed);
}

double txrng_next()
{
    return (double)_txrng_xorshift128_next(default_rng) / UINT32_MAX;
}

int32_t txrng_range(int32_t min, int32_t max)
{
    return (int32_t)(txrng_next() * (max - min)) + min;
}

float txrng_rangef(float min, float max)
{
    double next = txrng_next();
    return (float)next * (max - min) + min;
}

#endif