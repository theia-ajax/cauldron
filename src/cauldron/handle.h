#pragma once

#include "tx_types.h"

typedef uint32_t raw_handle;

enum { INVALID_RAW_HANDLE = 0 };

inline raw_handle raw_handle_make(raw_handle index, raw_handle gen)
{
    return (gen << 16) + (index & 0xFFFF);
}

inline raw_handle raw_handle_get_index(raw_handle handle)
{
    return handle & 0xFFFF;
}

inline uint16_t raw_handle_get_gen(raw_handle handle)
{
    return (uint16_t)(handle >> 16);
}

inline bool raw_handle_is_valid(raw_handle handle, raw_handle* handles, size_t len)
{
    if (handle != INVALID_RAW_HANDLE) {
        raw_handle index = raw_handle_get_index(handle);
        if (VALID_INDEX(index, len)) {
            return handle == handles[index];
        }
    }

    return false;
}

#define HANDLE(type) type##_handle

#define INVALID_HANDLE(type)                                                                       \
    (HANDLE(type))                                                                                 \
    {                                                                                              \
        INVALID_RAW_HANDLE                                                                         \
    }

#define VALID_HANDLE(handle) (handle.value != INVALID_RAW_HANDLE)

#define HANDLE_FUNC(type, func) type##_handle_##func

#define DEFINE_HANDLE_MAKE_FUNC(type)                                                              \
    inline HANDLE(type) HANDLE_FUNC(type, make)(uint32_t index, uint32_t gen)                      \
    {                                                                                              \
        return (HANDLE(type)){.value = raw_handle_make(index, gen)};                               \
    }

#define DEFINE_HANDLE_GET_INDEX_FUNC(type)                                                         \
    inline uint32_t HANDLE_FUNC(type, get_index)(HANDLE(type) handle)                              \
    {                                                                                              \
        return raw_handle_get_index(handle.value);                                                 \
    }

#define DEFINE_HANDLE_GET_GEN_FUNC(type)                                                           \
    inline uint16_t HANDLE_FUNC(type, get_gen)(HANDLE(type) handle)                                \
    {                                                                                              \
        return raw_handle_get_gen(handle.value);                                                   \
    }

#define DEFINE_HANDLE_VALID_IN_FUNC(type)                                                          \
    inline bool HANDLE_FUNC(type, valid_in)(                                                       \
        HANDLE(type) handle, HANDLE(type) * handles, size_t len)                                   \
    {                                                                                              \
        return raw_handle_is_valid(handle.value, (raw_handle*)handles, len);                       \
    }

#define DEFINE_HANDLE_VALID_FUNC(type)                                                             \
    inline bool HANDLE_FUNC(type, valid)(HANDLE(type) handle)                                      \
    {                                                                                              \
        return raw_handle_is_valid(                                                                \
            handle.value,                                                                          \
            (raw_handle*)type##_system_get_handles(),                                              \
            type##_system_get_handles_len());                                                      \
    }

#define DEFINE_HANDLE(type)                                                                        \
    typedef struct HANDLE(type) {                                                                  \
        raw_handle value;                                                                          \
    } HANDLE(type);                                                                                \
    HANDLE(type) * type##_system_get_handles(void);                                                \
    size_t type##_system_get_handles_len(void);                                                    \
    DEFINE_HANDLE_MAKE_FUNC(type)                                                                  \
    DEFINE_HANDLE_GET_INDEX_FUNC(type)                                                             \
    DEFINE_HANDLE_GET_GEN_FUNC(type)                                                               \
    DEFINE_HANDLE_VALID_IN_FUNC(type)                                                              \
    DEFINE_HANDLE_VALID_FUNC(type)
