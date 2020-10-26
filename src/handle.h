#pragma once

#define HANDLE(type) type##_handle

#define RAW_INVALID_HANDLE 0
#define INVALID_HANDLE(type)                                                                       \
    (HANDLE(type))                                                                                 \
    {                                                                                              \
        RAW_INVALID_HANDLE                                                                         \
    }

#define DEFINE_HANDLE(type)                                                                        \
    typedef struct HANDLE(type) {                                                                  \
        uint32_t value;                                                                            \
    } HANDLE(type);

#define VALID_HANDLE(handle) (handle.value != 0)