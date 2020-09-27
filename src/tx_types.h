#pragma once

#include "tx_system.h"
#include <stdbool.h>
#include <stdint.h>

// clang-format off
#define TX_ASSERT_ALWAYS(expression)                                                               \
    {                                                                                              \
        if ((expression)) {                                                                        \
        } else {                                                                                   \
            _tx_internal_print_assert(__FILE__, __LINE__ , #expression);                           \
            DEBUG_BREAK                                                                            \
        }                                                                                          \
    }
// clang-format on

#if _DEBUG
#define TX_ASSERT(expression) TX_ASSERT_ALWAYS(expression)
#else
#define TX_ASSERT(expresion, ...)
#endif

#define VALID_INDEX(index, count) ((size_t)(index) < (size_t)(count))