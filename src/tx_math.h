#pragma once

#include "tx_types.h"
#include <math.h>

#define TX_PI 3.14159265358979f

struct vector2 {
    union {
        float v[2];
        struct {
            float x, y;
        };
    };
};

struct vector3 {
    union {
        float v[3];
        struct {
            float x, y, z;
        };
    };
};

struct vector4 {
    union {
        float v[4];
        struct {
            float x, y, z, w;
        };
    };
};

struct quaternion {
    union {
        float v[4];
        struct {
            float x, y, z, w;
        };
    };
};

struct matrix4x4 {
    union {
        float m[4][4];
        float d[16];
        struct {
            float m11, m12, m13, m14;
            float m21, m22, m23, m24;
            float m31, m32, m33, m34;
            float m41, m42, m43, m44;
        };
    };
};

typedef struct vector2 vec2;
typedef struct vector3 vec3;
typedef struct vector4 vec4;
typedef struct quaternion quat;
typedef struct matrix4x4 mat4;

float clampf(const float v, const float min, const float max);
float lerpf(const float a, const float b, const float t);

vec2 vec2_add(const vec2 a, const vec2 b);
vec2 vec2_sub(const vec2 a, const vec2 b);
vec2 vec2_scale(const vec2 v, const float s);
float vec2_dot(const vec2 a, const vec2 b);
float vec2_len(const vec2 v);
float vec2_len2(const vec2 v);
vec2 vec2_norm(const vec2 v);
vec2 vec2_min(const vec2 a, const vec2 b);
vec2 vec2_max(const vec2 a, const vec2 b);
vec2 vec2_clamp(const vec2 a, const vec2 min, const vec2 max);
vec2 vec2_lerp(const vec2 a, const vec2 b, const float t);

vec3 vec3_add(const vec3 a, const vec3 b);
vec3 vec3_sub(const vec3 a, const vec3 b);
vec3 vec3_scale(const vec3 v, const float s);
float vec3_dot(const vec3 a, const vec3 b);
float vec3_len(const vec3 v);
float vec3_len2(const vec3 v);
vec3 vec3_norm(const vec3 v);
vec3 vec3_min(const vec3 a, const vec3 b);
vec3 vec3_max(const vec3 a, const vec3 b);
vec3 vec3_clamp(const vec3 a, const vec3 min, const vec3 max);
vec3 vec3_lerp(const vec3 a, const vec3 b, const float t);
vec3 vec3_cross(const vec3 a, const vec3 b);
vec3 vec3_reflect(const vec3 v, const vec3 n);

vec4 vec4_add(const vec4 a, const vec4 b);
vec4 vec4_sub(const vec4 a, const vec4 b);
vec4 vec4_scale(const vec4 v, const float s);
float vec4_dot(const vec4 a, const vec4 b);
float vec4_len(const vec4 v);
float vec4_len2(const vec4 v);
vec4 vec4_norm(const vec4 v);
vec4 vec4_min(const vec4 a, const vec4 b);
vec4 vec4_max(const vec4 a, const vec4 b);
vec4 vec4_clamp(const vec4 a, const vec4 min, const vec4 max);
vec4 vec4_lerp(const vec4 a, const vec4 b, const float t);
vec4 vec4_cross(const vec4 a, const vec4 b);
vec4 vec4_reflect(const vec4 v, const vec4 n);

mat4 mat4_identity();
vec4 mat4_row(const mat4 m, uint32_t i);
vec4 mat4_col(const mat4 m, uint32_t i);
mat4 mat4_transpose(const mat4 m);
mat4 mat4_add(const mat4 a, const mat4 b);
mat4 mat4_sub(const mat4 a, const mat4 b);
mat4 mat4_scale(const mat4 m, float s);
mat4 mat4_scale_aniso(const mat4 a, float x, float y, float z);
mat4 mat4_mul(const mat4 a, const mat4 b);
vec4 mat4_mul_vec4(const mat4 m, const vec4 v);
mat4 mat4_translate(const mat4 m, float x, float y, float z);
mat4 mat4_from_vec3_mul_outer(const vec3 a, const vec3 b);
mat4 mat4_rotate(const mat4 m, float x, float y, float z, float radians);
mat4 mat4_rotate_X(const mat4 m, float radians);
mat4 mat4_rotate_Y(const mat4 m, float radians);
mat4 mat4_rotate_Z(const mat4 m, float radians);
mat4 mat4_invert(const mat4 m);
mat4 mat4_orthonormalize(const mat4 m);
mat4 mat4_frustum(float l, float r, float b, float t, float n, float f);
mat4 mat4_ortho(float l, float r, float b, float t, float n, float f);
mat4 mat4_perspective(float y_fov, float aspect, float n, float f);
mat4 mat4_look_at(const vec3 eye, const vec3 center, const vec3 up);

// TODO: REMOVE NO COMMIT
#define TX_MATH_IMPL

// Implementation
#ifdef TX_MATH_IMPL

// math utilities
float clampf(const float v, const float min, const float max)
{
    return (v < min) ? min : (v > max) ? max : v;
}

float lerpf(const float a, const float b, const float t)
{
    return (b - a) * t + a;
}

// vec2
vec2 vec2_add(const vec2 a, const vec2 b)
{
    return (vec2){
        .x = a.x + b.x,
        .y = a.y + b.y,
    };
}

vec2 vec2_sub(const vec2 a, const vec2 b)
{
    return (vec2){
        .x = a.x - b.x,
        .y = a.y - b.y,
    };
}

vec2 vec2_scale(const vec2 v, const float s)
{
    return (vec2){
        .x = v.x * s,
        .y = v.y * s,
    };
}

float vec2_dot(const vec2 a, const vec2 b)
{
    return a.x * b.x + a.y * b.y;
}

float vec2_len(const vec2 v)
{
    return sqrtf(vec2_dot(v, v));
}

float vec2_len2(const vec2 v)
{
    return vec2_dot(v, v);
}

vec2 vec2_norm(const vec2 v)
{
    return vec2_scale(v, 1.0f / vec2_len(v));
}

vec2 vec2_min(const vec2 a, const vec2 b)
{
    return (vec2){
        .x = fminf(a.x, b.x),
        .y = fminf(a.y, b.y),
    };
}

vec2 vec2_max(const vec2 a, const vec2 b)
{
    return (vec2){
        .x = fmaxf(a.x, b.x),
        .y = fmaxf(a.y, b.y),
    };
}

vec2 vec2_clamp(const vec2 v, const vec2 min, const vec2 max)
{
    return (vec2){
        .x = clampf(v.x, min.x, max.x),
        .y = clampf(v.y, min.x, max.x),
    };
}

vec2 vec2_lerp(const vec2 a, const vec2 b, const float t)
{
    return (vec2){
        .x = lerpf(a.x, b.x, t),
        .y = lerpf(a.y, b.y, t),
    };
}

// vec3
vec3 vec3_add(const vec3 a, const vec3 b)
{
    return (vec3){
        .x = a.x + b.x,
        .y = a.y + b.y,
        .z = a.z + b.z,
    };
}

vec3 vec3_sub(const vec3 a, const vec3 b)
{
    return (vec3){
        .x = a.x - b.x,
        .y = a.y - b.y,
        .z = a.z - b.z,
    };
}

vec3 vec3_scale(const vec3 v, const float s)
{
    return (vec3){
        .x = v.x * s,
        .y = v.y * s,
        .z = v.z * s,
    };
}

float vec3_dot(const vec3 a, const vec3 b)
{
    return a.x * b.x + a.y * b.y * a.z + b.z;
}

float vec3_len(const vec3 v)
{
    return sqrtf(vec3_dot(v, v));
}

float vec3_len2(const vec3 v)
{
    return vec3_dot(v, v);
}

vec3 vec3_norm(const vec3 v)
{
    return vec3_scale(v, 1.0f / vec3_len(v));
}

vec3 vec3_min(const vec3 a, const vec3 b)
{
    return (vec3){
        .x = fminf(a.x, b.x),
        .y = fminf(a.y, b.y),
        .z = fminf(a.z, b.z),
    };
}

vec3 vec3_max(const vec3 a, const vec3 b)
{
    return (vec3){
        .x = fmaxf(a.x, b.x),
        .y = fmaxf(a.y, b.y),
        .z = fmaxf(a.z, b.z),
    };
}

vec3 vec3_clamp(const vec3 v, const vec3 min, const vec3 max)
{
    return (vec3){
        .x = clampf(v.x, min.x, max.x),
        .y = clampf(v.y, min.x, max.x),
        .z = clampf(v.z, min.z, max.z),
    };
}

vec3 vec3_lerp(const vec3 a, const vec3 b, const float t)
{
    return (vec3){
        .x = lerpf(a.x, b.x, t),
        .y = lerpf(a.y, b.y, t),
        .z = lerpf(a.z, b.z, t),
    };
}

vec3 vec3_cross(const vec3 a, const vec3 b)
{
    return (vec3){
        .x = a.v[1] * b.v[2] - a.v[2] * b.v[1],
        .y = a.v[2] * b.v[0] - a.v[0] * b.v[2],
        .z = a.v[0] * b.v[1] - a.v[1] * b.v[0],
    };
}

vec3 vec3_reflect(const vec3 v, const vec3 n)
{
    float p = 2.0f * vec3_dot(v, n);
    return (vec3){
        .x = v.x - p * n.x,
        .y = v.y - p * n.y,
        .z = v.z - p * n.z,
    };
}

// vec4
vec4 vec4_add(const vec4 a, const vec4 b)
{
    return (vec4){
        .x = a.x + b.x,
        .y = a.y + b.y,
        .z = a.z + b.z,
        .w = a.w + b.w,
    };
}

vec4 vec4_sub(const vec4 a, const vec4 b)
{
    return (vec4){
        .x = a.x - b.x,
        .y = a.y - b.y,
        .z = a.z - b.z,
        .w = a.w - b.w,
    };
}

vec4 vec4_scale(const vec4 v, const float s)
{
    return (vec4){
        .x = v.x * s,
        .y = v.y * s,
        .z = v.z * s,
        .w = v.w * s,
    };
}

float vec4_dot(const vec4 a, const vec4 b)
{
    return a.x * b.x + a.y * b.y * a.z + b.z + a.w * b.w;
}

float vec4_len(const vec4 v)
{
    return sqrtf(vec4_dot(v, v));
}

float vec4_len2(const vec4 v)
{
    return vec4_dot(v, v);
}

vec4 vec4_norm(const vec4 v)
{
    return vec4_scale(v, 1.0f / vec4_len(v));
}

vec4 vec4_min(const vec4 a, const vec4 b)
{
    return (vec4){
        .x = fminf(a.x, b.x),
        .y = fminf(a.y, b.y),
        .z = fminf(a.z, b.z),
        .w = fminf(a.w, b.w),
    };
}

vec4 vec4_max(const vec4 a, const vec4 b)
{
    return (vec4){
        .x = fmaxf(a.x, b.x),
        .y = fmaxf(a.y, b.y),
        .z = fmaxf(a.z, b.z),
        .w = fmaxf(a.w, b.w),
    };
}

vec4 vec4_clamp(const vec4 v, const vec4 min, const vec4 max)
{
    return (vec4){
        .x = clampf(v.x, min.x, max.x),
        .y = clampf(v.y, min.x, max.x),
        .z = clampf(v.z, min.z, max.z),
        .w = clampf(v.w, min.w, max.w),
    };
}

vec4 vec4_lerp(const vec4 a, const vec4 b, const float t)
{
    return (vec4){
        .x = lerpf(a.x, b.x, t),
        .y = lerpf(a.y, b.y, t),
        .z = lerpf(a.z, b.z, t),
        .w = lerpf(a.w, b.w, t),
    };
}

vec4 vec4_cross(const vec4 a, const vec4 b)
{
    return (vec4){
        .x = a.v[1] * b.v[2] - a.v[2] * b.v[1],
        .y = a.v[2] * b.v[0] - a.v[0] * b.v[2],
        .z = a.v[0] * b.v[1] - a.v[1] * b.v[0],
        .w = 1.0f,
    };
}

vec4 vec4_reflect(const vec4 v, const vec4 n)
{
    float p = 2.0f * vec4_dot(v, n);
    return (vec4){
        .x = v.x - p * n.x,
        .y = v.y - p * n.y,
        .z = v.z - p * n.z,
        .w = v.w * p * n.w,
    };
}

// mat4
mat4 mat4_identity()
{
    return (mat4){
        .m11 = 1.0f,
        .m22 = 1.0f,
        .m33 = 1.0f,
        .m44 = 1.0f,
    };
}

vec4 mat4_row(const mat4 m, uint32_t i)
{
    TX_ASSERT(VALID_INDEX(i, 4));

    return (vec4){
        .x = m.m[0][i],
        .y = m.m[1][i],
        .z = m.m[2][i],
        .w = m.m[3][i],
    };
}

vec4 mat4_col(const mat4 m, uint32_t i)
{
    TX_ASSERT(VALID_INDEX(i, 4));

    return (vec4){
        .x = m.m[i][0],
        .y = m.m[i][1],
        .z = m.m[i][2],
        .w = m.m[i][3],
    };
}

mat4 mat4_transpose(const mat4 m)
{
    mat4 result;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.m[i][j] = m.m[j][i];
        }
    }
    return result;
}

mat4 mat4_add(const mat4 a, const mat4 b)
{
    mat4 result = a;
    for (int i = 0; i < 16; ++i) {
        result.d[i] += b.d[i];
    }
    return result;
}

mat4 mat4_sub(const mat4 a, const mat4 b)
{
    mat4 result = a;
    for (int i = 0; i < 16; ++i) {
        result.d[i] -= b.d[i];
    }
    return result;
}

mat4 mat4_scale(const mat4 m, float s)
{
    mat4 result = m;
    for (int i = 0; i < 16; ++i) {
        result.d[i] *= s;
    }
    return result;
}

mat4 mat4_scale_aniso(const mat4 a, float x, float y, float z)
{
    mat4 result;
    float s[3] = {x, y, z};
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 4; ++c) {
            result.m[r][c] = a.m[r][c] * s[r];
        }
    }
    for (int c = 0; c < 4; ++c) {
        result.m[3][c] = a.m[3][c];
    }
    return result;
}

mat4 mat4_mul(const mat4 a, const mat4 b)
{
    mat4 result = {0};
    for (int c = 0; c < 4; ++c) {
        for (int r = 0; r < 4; ++r) {
            for (int k = 0; k < 4; ++k) {
                result.m[c][r] += a.m[k][r] * b.m[c][k];
            }
        }
    }
    return result;
}

vec4 mat4_mul_vec4(const mat4 m, const vec4 v)
{
    vec4 result;
    for (int c = 0; c < 4; ++c) {
        result.v[c] = 0.0f;
        for (int r = 0; r < 4; ++r) {
            result.v[c] += m.m[r][c] * v.v[c];
        }
    }
    return result;
}

mat4 mat4_translate(const mat4 m, float x, float y, float z)
{
    mat4 result = mat4_identity();
    result.m[3][0] = x;
    result.m[3][1] = y;
    result.m[3][2] = z;
    return result;
}

// mat4 mat4_from_vec3_mul_outer(const vec3 a, const vec3 b);
// mat4 mat4_rotate(const mat4 m, float x, float y, float z, float radians);
// mat4 mat4_rotate_X(const mat4 m, float radians);
// mat4 mat4_rotate_Y(const mat4 m, float radians);
// mat4 mat4_rotate_Z(const mat4 m, float radians);
// mat4 mat4_invert(const mat4 m);
// mat4 mat4_orthonormalize(const mat4 m);
// mat4 mat4_frustum(float l, float r, float b, float t, float n, float f);
// mat4 mat4_ortho(float l, float r, float b, float t, float n, float f);
// mat4 mat4_perspective(float y_fov, float aspect, float n, float f);
// mat4 mat4_look_at(const vec3 eye, const vec3 center, const vec3 up);
#endif

// static inline void mat4x4_from_vec3_mul_outer(mat4x4 M, vec3 a, vec3 b)
// {
//     int i, j;
//     for (i = 0; i < 4; ++i)
//         for (j = 0; j < 4; ++j)
//             M[i][j] = i < 3 && j < 3 ? a[i] * b[j] : 0.f;
// }
// static inline void mat4x4_rotate(mat4x4 R, mat4x4 M, float x, float y, float z, float angle)
// {
//     float s = sinf(angle);
//     float c = cosf(angle);
//     vec3 u = {x, y, z};

//     if (vec3_len(u) > 1e-4) {
//         vec3_norm(u, u);
//         mat4x4 T;
//         mat4x4_from_vec3_mul_outer(T, u, u);

//         mat4x4 S = {{0, u[2], -u[1], 0}, {-u[2], 0, u[0], 0}, {u[1], -u[0], 0, 0}, {0, 0, 0, 0}};
//         mat4x4_scale(S, S, s);

//         mat4x4 C;
//         mat4x4_identity(C);
//         mat4x4_sub(C, C, T);

//         mat4x4_scale(C, C, c);

//         mat4x4_add(T, T, C);
//         mat4x4_add(T, T, S);

//         T[3][3] = 1.;
//         mat4x4_mul(R, M, T);
//     } else {
//         mat4x4_dup(R, M);
//     }
// }
// static inline void mat4x4_rotate_X(mat4x4 Q, mat4x4 M, float angle)
// {
//     float s = sinf(angle);
//     float c = cosf(angle);
//     mat4x4 R = {{1.f, 0.f, 0.f, 0.f}, {0.f, c, s, 0.f}, {0.f, -s, c, 0.f}, {0.f, 0.f, 0.f, 1.f}};
//     mat4x4_mul(Q, M, R);
// }
// static inline void mat4x4_rotate_Y(mat4x4 Q, mat4x4 M, float angle)
// {
//     float s = sinf(angle);
//     float c = cosf(angle);
//     mat4x4 R = {{c, 0.f, -s, 0.f}, {0.f, 1.f, 0.f, 0.f}, {s, 0.f, c, 0.f}, {0.f, 0.f, 0.f, 1.f}};
//     mat4x4_mul(Q, M, R);
// }
// static inline void mat4x4_rotate_Z(mat4x4 Q, mat4x4 M, float angle)
// {
//     float s = sinf(angle);
//     float c = cosf(angle);
//     mat4x4 R = {{c, s, 0.f, 0.f}, {-s, c, 0.f, 0.f}, {0.f, 0.f, 1.f, 0.f}, {0.f, 0.f, 0.f, 1.f}};
//     mat4x4_mul(Q, M, R);
// }
// static inline void mat4x4_invert(mat4x4 T, mat4x4 M)
// {
//     float s[6];
//     float c[6];
//     s[0] = M[0][0] * M[1][1] - M[1][0] * M[0][1];
//     s[1] = M[0][0] * M[1][2] - M[1][0] * M[0][2];
//     s[2] = M[0][0] * M[1][3] - M[1][0] * M[0][3];
//     s[3] = M[0][1] * M[1][2] - M[1][1] * M[0][2];
//     s[4] = M[0][1] * M[1][3] - M[1][1] * M[0][3];
//     s[5] = M[0][2] * M[1][3] - M[1][2] * M[0][3];

//     c[0] = M[2][0] * M[3][1] - M[3][0] * M[2][1];
//     c[1] = M[2][0] * M[3][2] - M[3][0] * M[2][2];
//     c[2] = M[2][0] * M[3][3] - M[3][0] * M[2][3];
//     c[3] = M[2][1] * M[3][2] - M[3][1] * M[2][2];
//     c[4] = M[2][1] * M[3][3] - M[3][1] * M[2][3];
//     c[5] = M[2][2] * M[3][3] - M[3][2] * M[2][3];

//     /* Assumes it is invertible */
//     float idet =
//         1.0f / (s[0] * c[5] - s[1] * c[4] + s[2] * c[3] + s[3] * c[2] - s[4] * c[1] + s[5] *
//         c[0]);

//     T[0][0] = (M[1][1] * c[5] - M[1][2] * c[4] + M[1][3] * c[3]) * idet;
//     T[0][1] = (-M[0][1] * c[5] + M[0][2] * c[4] - M[0][3] * c[3]) * idet;
//     T[0][2] = (M[3][1] * s[5] - M[3][2] * s[4] + M[3][3] * s[3]) * idet;
//     T[0][3] = (-M[2][1] * s[5] + M[2][2] * s[4] - M[2][3] * s[3]) * idet;

//     T[1][0] = (-M[1][0] * c[5] + M[1][2] * c[2] - M[1][3] * c[1]) * idet;
//     T[1][1] = (M[0][0] * c[5] - M[0][2] * c[2] + M[0][3] * c[1]) * idet;
//     T[1][2] = (-M[3][0] * s[5] + M[3][2] * s[2] - M[3][3] * s[1]) * idet;
//     T[1][3] = (M[2][0] * s[5] - M[2][2] * s[2] + M[2][3] * s[1]) * idet;

//     T[2][0] = (M[1][0] * c[4] - M[1][1] * c[2] + M[1][3] * c[0]) * idet;
//     T[2][1] = (-M[0][0] * c[4] + M[0][1] * c[2] - M[0][3] * c[0]) * idet;
//     T[2][2] = (M[3][0] * s[4] - M[3][1] * s[2] + M[3][3] * s[0]) * idet;
//     T[2][3] = (-M[2][0] * s[4] + M[2][1] * s[2] - M[2][3] * s[0]) * idet;

//     T[3][0] = (-M[1][0] * c[3] + M[1][1] * c[1] - M[1][2] * c[0]) * idet;
//     T[3][1] = (M[0][0] * c[3] - M[0][1] * c[1] + M[0][2] * c[0]) * idet;
//     T[3][2] = (-M[3][0] * s[3] + M[3][1] * s[1] - M[3][2] * s[0]) * idet;
//     T[3][3] = (M[2][0] * s[3] - M[2][1] * s[1] + M[2][2] * s[0]) * idet;
// }
// static inline void mat4x4_orthonormalize(mat4x4 R, mat4x4 M)
// {
//     mat4x4_dup(R, M);
//     float s = 1.;
//     vec3 h;

//     vec3_norm(R[2], R[2]);

//     s = vec3_mul_inner(R[1], R[2]);
//     vec3_scale(h, R[2], s);
//     vec3_sub(R[1], R[1], h);
//     vec3_norm(R[1], R[1]);

//     s = vec3_mul_inner(R[0], R[2]);
//     vec3_scale(h, R[2], s);
//     vec3_sub(R[0], R[0], h);

//     s = vec3_mul_inner(R[0], R[1]);
//     vec3_scale(h, R[1], s);
//     vec3_sub(R[0], R[0], h);
//     vec3_norm(R[0], R[0]);
// }

// static inline void mat4x4_frustum(mat4x4 M, float l, float r, float b, float t, float n, float f)
// {
//     M[0][0] = 2.f * n / (r - l);
//     M[0][1] = M[0][2] = M[0][3] = 0.f;

//     M[1][1] = 2. * n / (t - b);
//     M[1][0] = M[1][2] = M[1][3] = 0.f;

//     M[2][0] = (r + l) / (r - l);
//     M[2][1] = (t + b) / (t - b);
//     M[2][2] = -(f + n) / (f - n);
//     M[2][3] = -1.f;

//     M[3][2] = -2.f * (f * n) / (f - n);
//     M[3][0] = M[3][1] = M[3][3] = 0.f;
// }
// static inline void mat4x4_ortho(mat4x4 M, float l, float r, float b, float t, float n, float f)
// {
//     M[0][0] = 2.f / (r - l);
//     M[0][1] = M[0][2] = M[0][3] = 0.f;

//     M[1][1] = 2.f / (t - b);
//     M[1][0] = M[1][2] = M[1][3] = 0.f;

//     M[2][2] = -2.f / (f - n);
//     M[2][0] = M[2][1] = M[2][3] = 0.f;

//     M[3][0] = -(r + l) / (r - l);
//     M[3][1] = -(t + b) / (t - b);
//     M[3][2] = -(f + n) / (f - n);
//     M[3][3] = 1.f;
// }
// static inline void mat4x4_perspective(mat4x4 m, float y_fov, float aspect, float n, float f)
// {
//     /* NOTE: Degrees are an unhandy unit to work with.
//      * linmath.h uses radians for everything! */
//     float const a = 1.f / tan(y_fov / 2.f);

//     m[0][0] = a / aspect;
//     m[0][1] = 0.f;
//     m[0][2] = 0.f;
//     m[0][3] = 0.f;

//     m[1][0] = 0.f;
//     m[1][1] = a;
//     m[1][2] = 0.f;
//     m[1][3] = 0.f;

//     m[2][0] = 0.f;
//     m[2][1] = 0.f;
//     m[2][2] = -((f + n) / (f - n));
//     m[2][3] = -1.f;

//     m[3][0] = 0.f;
//     m[3][1] = 0.f;
//     m[3][2] = -((2.f * f * n) / (f - n));
//     m[3][3] = 0.f;
// }
// static inline void mat4x4_look_at(mat4x4 m, vec3 eye, vec3 center, vec3 up)
// {
//     /* Adapted from Android's OpenGL Matrix.java.                        */
//     /* See the OpenGL GLUT documentation for gluLookAt for a description */
//     /* of the algorithm. We implement it in a straightforward way:       */

//     /* TODO: The negation of of can be spared by swapping the order of
//      *       operands in the following cross products in the right way. */
//     vec3 f;
//     vec3_sub(f, center, eye);
//     vec3_norm(f, f);

//     vec3 s;
//     vec3_mul_cross(s, f, up);
//     vec3_norm(s, s);

//     vec3 t;
//     vec3_mul_cross(t, s, f);

//     m[0][0] = s[0];
//     m[0][1] = t[0];
//     m[0][2] = -f[0];
//     m[0][3] = 0.f;

//     m[1][0] = s[1];
//     m[1][1] = t[1];
//     m[1][2] = -f[1];
//     m[1][3] = 0.f;

//     m[2][0] = s[2];
//     m[2][1] = t[2];
//     m[2][2] = -f[2];
//     m[2][3] = 0.f;

//     m[3][0] = 0.f;
//     m[3][1] = 0.f;
//     m[3][2] = 0.f;
//     m[3][3] = 1.f;

//     mat4x4_translate_in_place(m, -eye[0], -eye[1], -eye[2]);
// }

// typedef float quat[4];
// static inline void quat_identity(quat q)
// {
//     q[0] = q[1] = q[2] = 0.f;
//     q[3] = 1.f;
// }
// static inline void quat_add(quat r, quat a, quat b)
// {
//     int i;
//     for (i = 0; i < 4; ++i)
//         r[i] = a[i] + b[i];
// }
// static inline void quat_sub(quat r, quat a, quat b)
// {
//     int i;
//     for (i = 0; i < 4; ++i)
//         r[i] = a[i] - b[i];
// }
// static inline void quat_mul(quat r, quat p, quat q)
// {
//     vec3 w;
//     vec3_mul_cross(r, p, q);
//     vec3_scale(w, p, q[3]);
//     vec3_add(r, r, w);
//     vec3_scale(w, q, p[3]);
//     vec3_add(r, r, w);
//     r[3] = p[3] * q[3] - vec3_mul_inner(p, q);
// }
// static inline void quat_scale(quat r, quat v, float s)
// {
//     int i;
//     for (i = 0; i < 4; ++i)
//         r[i] = v[i] * s;
// }
// static inline float quat_inner_product(quat a, quat b)
// {
//     float p = 0.f;
//     int i;
//     for (i = 0; i < 4; ++i)
//         p += b[i] * a[i];
//     return p;
// }
// static inline void quat_conj(quat r, quat q)
// {
//     int i;
//     for (i = 0; i < 3; ++i)
//         r[i] = -q[i];
//     r[3] = q[3];
// }
// static inline void quat_rotate(quat r, float angle, vec3 axis)
// {
//     vec3 v;
//     vec3_scale(v, axis, sinf(angle / 2));
//     int i;
//     for (i = 0; i < 3; ++i)
//         r[i] = v[i];
//     r[3] = cosf(angle / 2);
// }
// #define quat_norm vec4_norm
// static inline void quat_mul_vec3(vec3 r, quat q, vec3 v)
// {
//     /*
//      * Method by Fabian 'ryg' Giessen (of Farbrausch)
//     t = 2 * cross(q.xyz, v)
//     v' = v + q.w * t + cross(q.xyz, t)
//      */
//     vec3 t;
//     vec3 q_xyz = {q[0], q[1], q[2]};
//     vec3 u = {q[0], q[1], q[2]};

//     vec3_mul_cross(t, q_xyz, v);
//     vec3_scale(t, t, 2);

//     vec3_mul_cross(u, q_xyz, t);
//     vec3_scale(t, t, q[3]);

//     vec3_add(r, v, t);
//     vec3_add(r, r, u);
// }
// static inline void mat4x4_from_quat(mat4x4 M, quat q)
// {
//     float a = q[3];
//     float b = q[0];
//     float c = q[1];
//     float d = q[2];
//     float a2 = a * a;
//     float b2 = b * b;
//     float c2 = c * c;
//     float d2 = d * d;

//     M[0][0] = a2 + b2 - c2 - d2;
//     M[0][1] = 2.f * (b * c + a * d);
//     M[0][2] = 2.f * (b * d - a * c);
//     M[0][3] = 0.f;

//     M[1][0] = 2 * (b * c - a * d);
//     M[1][1] = a2 - b2 + c2 - d2;
//     M[1][2] = 2.f * (c * d + a * b);
//     M[1][3] = 0.f;

//     M[2][0] = 2.f * (b * d + a * c);
//     M[2][1] = 2.f * (c * d - a * b);
//     M[2][2] = a2 - b2 - c2 + d2;
//     M[2][3] = 0.f;

//     M[3][0] = M[3][1] = M[3][2] = 0.f;
//     M[3][3] = 1.f;
// }

// static inline void mat4x4o_mul_quat(mat4x4 R, mat4x4 M, quat q)
// {
//     /*  XXX: The way this is written only works for othogonal matrices. */
//     /* TODO: Take care of non-orthogonal case. */
//     quat_mul_vec3(R[0], q, M[0]);
//     quat_mul_vec3(R[1], q, M[1]);
//     quat_mul_vec3(R[2], q, M[2]);

//     R[3][0] = R[3][1] = R[3][2] = 0.f;
//     R[3][3] = 1.f;
// }
// static inline void quat_from_mat4x4(quat q, mat4x4 M)
// {
//     float r = 0.f;
//     int i;

//     int perm[] = {0, 1, 2, 0, 1};
//     int *p = perm;

//     for (i = 0; i < 3; i++) {
//         float m = M[i][i];
//         if (m < r)
//             continue;
//         m = r;
//         p = &perm[i];
//     }

//     r = sqrtf(1.f + M[p[0]][p[0]] - M[p[1]][p[1]] - M[p[2]][p[2]]);

//     if (r < 1e-6) {
//         q[0] = 1.f;
//         q[1] = q[2] = q[3] = 0.f;
//         return;
//     }

//     q[0] = r / 2.f;
//     q[1] = (M[p[0]][p[1]] - M[p[1]][p[0]]) / (2.f * r);
//     q[2] = (M[p[2]][p[0]] - M[p[0]][p[2]]) / (2.f * r);
//     q[3] = (M[p[2]][p[1]] - M[p[1]][p[2]]) / (2.f * r);
// }

// static inline void mat4x4_arcball(mat4x4 R, mat4x4 M, vec2 _a, vec2 _b, float s)
// {
//     vec2 a;
//     memcpy(a, _a, sizeof(a));
//     vec2 b;
//     memcpy(b, _b, sizeof(b));

//     float z_a = 0.;
//     float z_b = 0.;

//     if (vec2_len(a) < 1.) {
//         z_a = sqrtf(1. - vec2_mul_inner(a, a));
//     } else {
//         vec2_norm(a, a);
//     }

//     if (vec2_len(b) < 1.) {
//         z_b = sqrtf(1. - vec2_mul_inner(b, b));
//     } else {
//         vec2_norm(b, b);
//     }

//     vec3 a_ = {a[0], a[1], z_a};
//     vec3 b_ = {b[0], b[1], z_b};

//     vec3 c_;
//     vec3_mul_cross(c_, a_, b_);

//     float const angle = acos(vec3_mul_inner(a_, b_)) * s;
//     mat4x4_rotate(R, M, c_[0], c_[1], c_[2], angle);
// }
