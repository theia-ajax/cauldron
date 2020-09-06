#include "tx_system.h"

#include <stdio.h>

void _tx_internal_print_assert(const char* expression)
{
    printf("_ ASSERTION FAILED __________________________________________________________\n\n");
    printf("  %s\n", expression);
}