#pragma once

#define POOL(type) type##_pool

#define POOL_SET_CAPACITY_PROTO(type) void type##_pool_set_capacity(uint32_t capacity)
#define POOL_ACQUIRE_PROTO(type) HANDLE(type) type##_acquire()
#define POOL_RELEASE_PROTO(type) void type##_release(HANDLE(type) handle)
#define POOL_FREE_PROTO(type) void type##_pool_free()
#define POOL_GET_HANDLES_PROTO(type) HANDLE(type) * get_##type##_handles()
#define POOL_GET_HANDLES_LEN_PROTO(type) size_t get_##type##_handles_len()
#define POOL_GET_ELEMENT_PTR_PROTO(type) type* type##_get(HANDLE(type) handle)

#define POOL_SET_CAPACITY(type)                                                                    \
    POOL_SET_CAPACITY_PROTO(type)                                                                  \
    {                                                                                              \
        uint32_t prev_cap = (uint32_t)arrcap(POOL(type).data);                                     \
        if (capacity <= prev_cap) {                                                                \
            return;                                                                                \
        }                                                                                          \
        uint32_t new_elem = capacity - prev_cap;                                                   \
        arrsetlen(POOL(type).data, capacity);                                                      \
        arrsetlen(POOL(type).handles, capacity);                                                   \
        arrsetcap(POOL(type).free_queue, capacity);                                                \
        memset(POOL(type).data + prev_cap, 0, sizeof(type) * new_elem);                            \
        memset(POOL(type).handles + prev_cap, 0, sizeof(HANDLE(type)) * new_elem);                 \
        for (uint32_t i = capacity; i > prev_cap; --i) {                                           \
            arrput(POOL(type).free_queue, i - 1);                                                  \
        }                                                                                          \
    }

#define POOL_ACQUIRE(type)                                                                         \
    POOL_ACQUIRE_PROTO(type)                                                                       \
    {                                                                                              \
        if (arrlen(POOL(type).data) == 0) {                                                        \
            uint32_t cap = max((uint32_t)arrcap(POOL(type).data), 16);                             \
            type##_pool_set_capacity(cap + cap / 2);                                               \
        }                                                                                          \
        uint32_t index = arrpop(POOL(type).free_queue);                                            \
        HANDLE(type) handle = HANDLE_FUNC(type, make)(index, POOL(type).gen);                      \
        POOL(type).handles[index] = handle;                                                        \
        return handle;                                                                             \
    }

#define POOL_RELEASE(type)                                                                         \
    POOL_RELEASE_PROTO(type)                                                                       \
    {                                                                                              \
        if (HANDLE_FUNC(type, get_gen)(handle) >= POOL(type).gen) {                                \
            ++POOL(type).gen;                                                                      \
        }                                                                                          \
        uint32_t index = HANDLE_FUNC(type, get_index)(handle);                                     \
        arrput(POOL(type).free_queue, index);                                                      \
        POOL(type).handles[index] = INVALID_HANDLE(type);                                          \
    }

#define POOL_FREE(type)                                                                            \
    POOL_FREE_PROTO(type)                                                                          \
    {                                                                                              \
        arrfree(POOL(type).data);                                                                  \
        arrfree(POOL(type).handles);                                                               \
        arrfree(POOL(type).free_queue);                                                            \
    }

#define POOL_GET_HANDLES(type)                                                                     \
    POOL_GET_HANDLES_PROTO(type)                                                                   \
    {                                                                                              \
        return POOL(type).handles;                                                                 \
    }

#define POOL_GET_HANDLES_LEN(type)                                                                 \
    POOL_GET_HANDLES_LEN_PROTO(type)                                                               \
    {                                                                                              \
        return arrlen(POOL(type).handles);                                                         \
    }

#define POOL_GET_ELEMENT_PTR(type)                                                                 \
    POOL_GET_ELEMENT_PTR_PROTO(type)                                                               \
    {                                                                                              \
        if (HANDLE_FUNC(type, valid)(handle)) {                                                    \
            return &POOL(type).data[HANDLE_FUNC(type, get_index)(handle)];                         \
        }                                                                                          \
        return NULL;                                                                               \
    }

#define DECLARE_POOL(type)                                                                         \
    typedef struct type type;                                                                      \
    POOL_GET_HANDLES_PROTO(type);                                                                  \
    POOL_GET_HANDLES_LEN_PROTO(type);                                                              \
    POOL_GET_ELEMENT_PTR_PROTO(type);

#define DEFINE_POOL(type)                                                                          \
    struct POOL(type) {                                                                            \
        type* data;                                                                                \
        HANDLE(type) * handles;                                                                    \
        uint32_t* free_queue;                                                                      \
        uint16_t gen;                                                                              \
    } POOL(type) = {.gen = 1};                                                                     \
    POOL_SET_CAPACITY(type)                                                                        \
    POOL_ACQUIRE(type)                                                                             \
    POOL_RELEASE(type)                                                                             \
    POOL_FREE(type)                                                                                \
    POOL_GET_HANDLES(type)                                                                         \
    POOL_GET_HANDLES_LEN(type)                                                                     \
    POOL_GET_ELEMENT_PTR(type)