#include "tx_system.h"

#include <stdio.h>

void _tx_internal_print_assert(const char* filename, int line, const char* expression)
{
    printf("[ASSERTION FAILED] (%s) : %s#%d\n", expression, filename, line);
}