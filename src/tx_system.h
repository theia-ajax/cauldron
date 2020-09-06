#pragma once

#ifdef _MSC_VER
#define DEBUG_BREAK __debugbreak();
#else
#include <signal.h>
#define DEBUG_BREAK raise(SIGTRAP);
#endif

void _tx_internal_print_assert(const char* expression);
