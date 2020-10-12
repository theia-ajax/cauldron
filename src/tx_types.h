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

typedef enum tx_result {
    TX_INVALID = -1,
    TX_SUCCESS = 0,
    TX_FILE_ERROR,
    TX_ALLOCATION_ERROR,
    TX_PARSE_ERROR,
    TX_INVALID_PARAMTER,
} tx_result;