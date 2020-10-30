#pragma once

#include <stdint.h>

void profile_start(char* name);
uint64_t profile_stop(char* name);
uint64_t profile_get_last_ticks(char* name);
uint64_t get_ticks();
uint64_t get_frequency();