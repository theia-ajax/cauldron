#pragma once

#include "tx_types.h"

void txrng_seed(uint32_t seed);
double txrng_next();
int32_t txrng_range(int32_t min, int32_t max);
float txrng_rangef(float min, float max);

// implmementation
#if defined(TX_RAND_IMPLEMENTATION)

struct xorshift128_state {
    uint32_t a, b, c, d;
};

/* The state array must be initialized to not be all zero */
uint32_t xorshift128_next(struct xorshift128_state state)
{
    /* Algorithm "xor128" from p. 5 of Marsaglia, "Xorshift RNGs" */
    uint32_t t = state.d;

    uint32_t const s = state.a;
    state.d = state.c;
    state.c = state.b;
    state.b = s;

    t ^= t << 11;
    t ^= t >> 8;
    return state.a = t ^ s ^ (s >> 19);
}

struct xorshift128_state default_rng;

void txrng_seed(uint32_t seed)
{
    default_rng.a = seed;
    default_rng.b = seed;
    default_rng.c = seed;
    default_rng.d = seed;
}

double txrng_next()
{
    return (double)xorshift128_next(default_rng) / UINT32_MAX;
}

int32_t txrng_range(int32_t min, int32_t max)
{
    return (int32_t)(txrng_next() * (max - min)) + min;
}

float txrng_rangef(float min, float max)
{
    return (float)txrng_next() * (max - min) + min;
}

#endif