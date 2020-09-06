#pragma once

#include "tx_system.h"
#include <stdbool.h>
#include <stdint.h>

#define TX_ASSERT_ALWAYS(expression)                                                               \
    {                                                                                              \
        static bool s_skip = false;                                                                \
        if (!s_skip) {                                                                             \
            s_skip = true;                                                                         \
            if (!(expression)) {                                                                   \
                _tx_internal_print_assert(#expression);                                            \
                DEBUG_BREAK                                                                        \
            }                                                                                      \
        }                                                                                          \
    }

#if _DEBUG
#define TX_ASSERT(expression) TX_ASSERT_ALWAYS(expression)
#else
#define TX_ASSERT(expresion, ...)
#endif

#define VALID_INDEX(index, count) (static_cast<size_t>(index) < static_cast<size_t>(count))