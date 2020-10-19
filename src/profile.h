#pragma once

#include <stdint.h>

void profile_start(const char* name);
uint64_t profile_stop(const char* name);