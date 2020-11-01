#pragma once

#include "tx_types.h"
#include <float.h>
#include <limits.h>
#include <math.h>

#define TX_PI 3.14159265358979f
#define TX_EPSILON 1e-4f;

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
        float q[4];
        struct {
            float x, y, z, w;
        };
        struct vector3 xyz;
        struct vector4 xyzw;
    };
};

struct matrix4x4 {
    union {
        float m[4][4];
        float d[16];
        struct vector4 v[4];
        struct {
            float m11, m12, m13, m14;
            float m21, m22, m23, m24;
            float m31, m32, m33, m34;
            float m41, m42, m43, m44;
        };
        struct {
            struct vector3 xyz;
            float w;
        } v3[4];
    };
};

typedef struct vector2 vec2;
typedef struct vector3 vec3;
typedef struct vector4 vec4;
typedef struct quaternion quat;
typedef struct matrix4x4 mat4;

float clampf(const float v, const float min, const float max);
float lerpf(const float a, const float b, const float t);
float nsinf(const float t); // normalized sin [0-1] result with normalized input [0-1] (1 tau)
float signf(const float v); // v < 0 -> -1, v == 0 > 0, v > 0 -> 1
#define mod(a, b) ((((a) % (b)) + (b)) % (b))

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
vec3 vec2_vec3(const vec2 v);
vec2 vec2_clamp_len(const vec2 v, const float l);
vec2 vec2_abs(const vec2 v);

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
vec3 vec3_clamp_len(const vec3 v, const float l);
vec3 vec3_abs(const vec3 v);

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
vec4 vec4_abs(const vec4 v);

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
mat4 mat4_from_vec3_mul(const vec3 a, const vec3 b);
mat4 mat4_rotate(const mat4 m, float x, float y, float z, float radians);
mat4 mat4_rotate_x(const mat4 m, float radians);
mat4 mat4_rotate_y(const mat4 m, float radians);
mat4 mat4_rotate_z(const mat4 m, float radians);
mat4 mat4_invert(const mat4 m);
mat4 mat4_orthonormalize(const mat4 m);
mat4 mat4_frustum(float l, float r, float b, float t, float n, float f);
mat4 mat4_ortho(float l, float r, float b, float t, float n, float f);
mat4 mat4_perspective(float y_fov_rad, float aspect, float n, float f);
mat4 mat4_look_at(const vec3 eye, const vec3 center, const vec3 up);
mat4 mat4_from_quat(const quat q);
mat4 mat4_arcball(const mat4 m, const vec2 a, const vec2 b, float s);

quat quat_identity();
quat quat_add(const quat a, const quat b);
quat quat_sub(const quat a, const quat b);
quat quat_mul(const quat p, const quat q);
quat quat_scale(const quat q, float s);
quat quat_norm(const quat q);
float quat_dot(const quat a, const quat b);
quat quat_conj(const quat q);
quat quat_rotate(float angle, const vec3 axis);
vec3 quat_mul_vec3(const quat q, const vec3 v);
quat quat_from_mat4x4(const mat4 m);

// Implementation
#ifdef TX_MATH_IMPLEMENTATION

#pragma region Basic Math Implementation

// math utilities
float clampf(const float v, const float min, const float max)
{
    return (v < min) ? min : (v > max) ? max : v;
}

float lerpf(const float a, const float b, const float t)
{
    return (b - a) * t + a;
}

float nsinf(const float t)
{
    return (sinf(TX_PI * 2.0f * t) + 1) / 2.0f;
}

float signf(const float v)
{
    return (v == 0.0f) ? 0.0f : (v < 0) ? -1.0f : 1.0f;
}

#pragma endregion

#pragma region Vec2 Implementation
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
    float l = vec2_len(v);
    return (l > 0) ? vec2_scale(v, 1.0f / l) : (vec2){0};
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

vec3 vec2_vec3(const vec2 v)
{
    return (vec3){
        .x = v.x,
        .y = v.y,
        .z = 0.0f,
    };
}

vec2 vec2_clamp_len(const vec2 v, const float l)
{
    float len = vec2_len(v);
    if (len > l) {
        return vec2_sub(v, vec2_scale(v, len - l));
    }
    return v;
}

vec2 vec2_abs(const vec2 v)
{
    return (vec2){
        .x = fabsf(v.x),
        .y = fabsf(v.y),
    };
}
#pragma endregion

#pragma region Vec3 Implementation
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
    return a.x * b.x + a.y * b.y + a.z * b.z;
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
    float l = vec3_len(v);
    return (l > 0) ? vec3_scale(v, 1.0f / l) : (vec3){0};
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

vec3 vec3_clamp_len(const vec3 v, const float l)
{
    float len = vec3_len(v);
    if (len > l) {
        return vec3_sub(v, vec3_scale(v, len - l));
    }
    return v;
}

vec3 vec3_abs(const vec3 v)
{
    return (vec3){
        .x = fabsf(v.x),
        .y = fabsf(v.y),
        .z = fabsf(v.z),
    };
}
#pragma endregion

#pragma region Vec4 Implementation
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
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
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
    float l = vec4_len(v);
    return (l > 0) ? vec4_scale(v, 1.0f / l) : (vec4){0};
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

vec4 vec4_abs(const vec4 v)
{
    return (vec4){
        .x = fabsf(v.x),
        .y = fabsf(v.y),
        .z = fabsf(v.z),
        .w = fabsf(v.w),
    };
}

#pragma endregion

#pragma region Mat4 Implementation
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
    result.m11 *= s;
    result.m22 *= s;
    result.m33 *= s;
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
    mat4 result = m;
    result.m41 = x;
    result.m42 = y;
    result.m43 = z;
    return result;
}

mat4 mat4_from_vec3_mul(const vec3 a, const vec3 b)
{
    mat4 result = {0};

    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            result.m[r][c] = a.v[r] * b.v[c];
        }
    }

    return result;
}

// mat4 mat4_from_vec3_mul_outer(const vec3 a, const vec3 b);
mat4 mat4_rotate(const mat4 m, float x, float y, float z, float radians)
{
    float s = sinf(radians);
    float c = cosf(radians);
    vec3 u = {x, y, z};

    if (vec3_len2(u) > 1e-8) {
        u = vec3_norm(u);
        mat4 mt = mat4_from_vec3_mul(u, u);
        mat4 ms = mat4_scale(
            (mat4){
                .m =
                    {
                        {0.0f, u.z, -u.y, 0},
                        {-u.z, 0.0f, u.x, 0.0f},
                        {u.y, -u.x, 0.f, 0.0f},
                        {0.0f, 0.0f, 0.0f, 0.0f},
                    },
            },
            s);
        mat4 mc = mat4_scale(mat4_sub(mat4_identity(), mt), c);

        mat4 temp = mat4_add(mat4_add(mt, mc), ms);
        temp.m33 = 1.0f;
        return mat4_mul(m, temp);
    } else {
        return m;
    }
}

mat4 mat4_rotate_x(const mat4 m, const float radians)
{
    float s = sinf(radians);
    float c = cosf(radians);
    return mat4_mul(
        m,
        (mat4){
            .m =
                {
                    {1.0f, 0.0f, 0.0f, 0.0f},
                    {0.0f, c, s, 0.0f},
                    {0.0f, -s, c, 0.0f},
                    {0.0f, 0.0f, 0.0f, 1.0f},
                },
        });
}

mat4 mat4_rotate_y(const mat4 m, float radians)
{
    float s = sinf(radians);
    float c = cosf(radians);
    return mat4_mul(
        m,
        (mat4){
            .m =
                {
                    {c, 0.0f, s, 0.0f},
                    {0.0f, 1.0f, 0.0f, 0.0f},
                    {-s, 0.0f, c, 0.0f},
                    {0.0f, 0.0f, 0.0f, 1.0f},
                },
        });
}

mat4 mat4_rotate_z(const mat4 m, float radians)
{
    float s = sinf(radians);
    float c = cosf(radians);
    return mat4_mul(
        m,
        (mat4){
            .m =
                {
                    {c, s, 0.0f, 0.0f},
                    {-s, c, 0.0f, 0.0f},
                    {0.0f, 0.0f, 1.0f, 0.0f},
                    {0.0f, 0.0f, 0.0f, 1.0f},
                },
        });
}

mat4 mat4_invert(const mat4 m)
{
    float t0[6];
    float t1[6];

    t0[0] = m.m11 * m.m22 - m.m21 * m.m12;
    t0[1] = m.m11 * m.m23 - m.m21 * m.m13;
    t0[2] = m.m11 * m.m24 - m.m21 * m.m14;
    t0[3] = m.m12 * m.m23 - m.m22 * m.m13;
    t0[4] = m.m12 * m.m24 - m.m22 * m.m14;
    t0[5] = m.m13 * m.m24 - m.m23 * m.m14;

    t1[0] = m.m31 * m.m42 - m.m41 * m.m32;
    t1[1] = m.m31 * m.m43 - m.m41 * m.m33;
    t1[2] = m.m31 * m.m44 - m.m41 * m.m34;
    t1[3] = m.m32 * m.m43 - m.m42 * m.m33;
    t1[4] = m.m32 * m.m44 - m.m42 * m.m34;
    t1[5] = m.m33 * m.m44 - m.m43 * m.m34;

    float inv_det = 1.0f
                    / (t0[0] * t1[5] - t0[1] * t1[4] + t0[2] * t1[3] + t0[3] * t1[2] - t0[4] * t1[1]
                       + t0[5] * t1[0]);

    mat4 result;
    result.m11 = (m.m22 * t1[5] - m.m23 * t1[4] + m.m24 * t1[3]) * inv_det;
    result.m12 = (-m.m12 * t1[5] - m.m13 * t1[4] + m.m14 * t1[3]) * inv_det;
    result.m13 = (m.m42 * t0[5] - m.m43 * t0[4] + m.m44 * t0[3]) * inv_det;
    result.m14 = (-m.m32 * t0[5] - m.m33 * t0[4] + m.m34 * t0[3]) * inv_det;

    result.m21 = (-m.m21 * t1[5] + m.m23 * t1[2] - m.m24 * t1[1]) * inv_det;
    result.m22 = (m.m11 * t1[5] - m.m13 * t1[2] + m.m14 * t1[1]) * inv_det;
    result.m23 = (-m.m41 * t0[5] + m.m43 * t0[2] - m.m44 * t0[1]) * inv_det;
    result.m24 = (m.m31 * t0[5] - m.m33 * t0[2] + m.m34 * t0[1]) * inv_det;

    result.m31 = (m.m21 * t1[4] - m.m22 * t1[2] + m.m24 * t1[0]) * inv_det;
    result.m32 = (-m.m11 * t1[4] + m.m13 * t1[2] - m.m14 * t1[0]) * inv_det;
    result.m33 = (m.m41 * t0[4] - m.m42 * t0[2] + m.m44 * t0[0]) * inv_det;
    result.m34 = (-m.m31 * t0[4] + m.m32 * t0[2] - m.m34 * t0[0]) * inv_det;

    result.m41 = (-m.m21 * t1[3] + m.m22 * t1[1] - m.m23 * t1[0]) * inv_det;
    result.m42 = (m.m11 * t1[3] - m.m12 * t1[1] + m.m13 * t1[0]) * inv_det;
    result.m43 = (-m.m41 * t0[3] + m.m42 * t0[1] - m.m43 * t0[0]) * inv_det;
    result.m44 = (m.m31 * t0[3] - m.m32 * t0[1] + m.m33 * t0[0]) * inv_det;

    return result;
}

mat4 mat4_orthonormalize(const mat4 m)
{
    mat4 r = m;

    r.v3[2].xyz = vec3_norm(r.v3[2].xyz);

    float s = vec3_dot(r.v3[1].xyz, r.v3[2].xyz);
    r.v3[1].xyz = vec3_norm(vec3_sub(r.v3[1].xyz, vec3_scale(r.v3[2].xyz, s)));

    s = vec3_dot(r.v3[0].xyz, r.v3[2].xyz);
    r.v3[0].xyz = vec3_sub(r.v3[0].xyz, vec3_scale(r.v3[2].xyz, s));

    s = vec3_dot(r.v3[0].xyz, r.v3[1].xyz);
    r.v3[0].xyz = vec3_norm(vec3_sub(r.v3[0].xyz, vec3_scale(r.v3[1].xyz, s)));

    return r;
}

mat4 mat4_frustum(float l, float r, float b, float t, float n, float f)
{
    mat4 result = {0};

    result.m11 = 2.0f * n / (r - l);
    result.m22 = 2.0f * n / (t - b);
    result.m31 = (r + l) / (r - l);
    result.m32 = (t + b) / (t - b);
    result.m33 = -(f + n) / (f - n);
    result.m34 = -1.0f;
    result.m43 = -2.0f * (f * n) / (f - n);

    return result;
}

mat4 mat4_ortho(float l, float r, float b, float t, float n, float f)
{
    mat4 result = {0};

    result.m11 = 2.f / (r - l);
    result.m22 = 2.f / (t - b);
    result.m33 = -2.f / (f - n);
    result.m41 = -(r + l) / (r - l);
    result.m42 = -(t + b) / (t - b);
    result.m43 = -(f + n) / (f - n);
    result.m44 = 1.0f;

    return result;
}

mat4 mat4_perspective(float y_fov_rad, float aspect, float n, float f)
{
    const float a = 1.0f / tanf(y_fov_rad / 2.0f);

    mat4 result = {0};

    result.m11 = a / aspect;
    result.m22 = a;
    result.m33 = -((f + n) / (f - n));
    result.m34 = -1.0f;
    result.m43 = -((2.f * f * n) / (f - n));

    return result;
}

mat4 mat4_look_at(const vec3 eye, const vec3 center, const vec3 up)
{
    vec3 f = vec3_norm(vec3_sub(center, eye));
    vec3 s = vec3_norm(vec3_cross(f, up));
    vec3 t = vec3_cross(s, f);

    mat4 result = mat4_identity();

    for (int i = 0; i < 3; ++i) {
        result.m[i][0] = s.v[i];
        result.m[i][1] = t.v[i];
        result.m[i][2] = -f.v[i];
    }

    return mat4_translate(result, -eye.x, -eye.y, -eye.z);
}

mat4 mat4_from_quat(const quat q)
{
    float a = q.w;
    float b = q.x;
    float c = q.y;
    float d = q.z;

    float a2 = a * a;
    float b2 = b * b;
    float c2 = c * c;
    float d2 = d * d;

    mat4 result = mat4_identity();
    result.m11 = a + b2 - c2 - d2;
    result.m12 = 2.0f * (b * c + a * d);
    result.m13 = 2.0f * (b * d - a * c);

    result.m21 = 2.0f * (b * c - a * d);
    result.m22 = a2 - b2 + c2 - d2;
    result.m23 = 2.0f * (c * d + a * b);

    result.m31 = 2.0f * (b * d + a * c);
    result.m32 = 2.0f * (c * d - a * b);
    result.m33 = a2 - b2 - c2 + d2;

    return result;
}

mat4 mat4_arcball(const mat4 m, const vec2 a, const vec2 b, float s)
{
    vec3 a3 = {.x = a.x, .y = a.y};
    vec3 b3 = {.x = b.x, .y = b.y};

    float al2 = vec2_len2(a);
    float bl2 = vec2_len2(b);

    if (vec2_len2(a) < 1.0f) {
        a3.z = sqrtf(1.0f - vec2_len2(a));
    } else {
        vec2 an = vec2_norm(a);
        a3.x = an.x;
        a3.y = an.y;
    }

    if (vec2_len2(b) < 1.0f) {
        b3.z = sqrtf(1.0f - vec2_len2(b));
    } else {
        vec2 bn = vec2_norm(b);
        b3.x = bn.x;
        b3.y = bn.y;
    }

    vec3 c = vec3_cross(a3, b3);
    const float radians = acosf(vec3_dot(a3, b3)) * s;
    return mat4_rotate(m, c.x, c.y, c.z, radians);
}
#pragma endregion

#pragma region Quat Implementation
quat quat_identity()
{
    // 0, 0, 0, 1
    return (quat){
        .q = {[3] = 1.0f},
    };
}

quat quat_add(const quat a, const quat b)
{
    return (quat){
        .x = a.x + b.x,
        .y = a.y + b.y,
        .z = a.z + b.z,
        .w = a.w + b.w,
    };
}

quat quat_sub(const quat a, const quat b)
{
    return (quat){
        .x = a.x - b.x,
        .y = a.y - b.y,
        .z = a.z - b.z,
        .w = a.w - b.w,
    };
}

quat quat_mul(const quat p, const quat q)
{
    vec3 r3 = vec3_cross(p.xyz, q.xyz);
    r3 = vec3_add(r3, vec3_scale(p.xyz, q.w));
    r3 = vec3_add(r3, vec3_scale(q.xyz, p.w));

    return (quat){
        .x = r3.x,
        .y = r3.y,
        .z = r3.z,
        .w = p.w * q.w - vec3_dot(p.xyz, q.xyz),
    };
}

quat quat_scale(const quat q, float s)
{
    vec4 scale = vec4_scale(q.xyzw, s);
    quat result;
    memcpy(result.q, scale.v, sizeof(result.q));
    return result;
}

quat quat_norm(const quat q)
{
    vec4 norm = vec4_norm(q.xyzw);
    quat result;
    memcpy(result.q, norm.v, sizeof(result.q));
    return result;
}

float quat_dot(const quat a, const quat b)
{
    return vec4_dot(a.xyzw, b.xyzw);
}

quat quat_conj(const quat q)
{
    return (quat){
        .x = -q.x,
        .y = -q.y,
        .z = -q.z,
        .w = q.w,
    };
}

quat quat_rotate(float radians, const vec3 axis)
{
    vec3 v = vec3_scale(axis, sinf(radians / 2.0f));
    return (quat){
        .x = v.x,
        .y = v.y,
        .z = v.z,
        .w = cosf(radians / 2.0f),
    };
}

vec3 quat_mul_vec3(const quat q, const vec3 v)
{
    // Method by Fabian 'ryg' Giessen (of Farbrausch)
    //  t = 2 * cross(q.xyz, v)
    //  v' = v + q.w * t + cross(q.xyz, t)

    vec3 t = vec3_scale(vec3_cross(q.xyz, v), 2.0f);
    vec3 u = vec3_cross(q.xyz, t);
    return vec3_add(vec3_add(v, vec3_scale(t, q.w)), u);
}

quat quat_from_mat4(const mat4 m)
{
    float r = 0.0f;
    int i0 = 0, i1, i2;

    for (int i = 0; i < 3; ++i) {
        float mv = m.m[i][i];
        if (mv >= r) {
            mv = r;
            i0 = i;
        }
    }

    i1 = (i0 + 1) % 3;
    i2 = (i0 + 2) % 3;

    r = sqrtf(1.0f + m.m[i0][i0] - m.m[i1][i1] - m.m[i2][i2]);

    if (r < 1e-6f) {
        return (quat){1.0f, 0.0f, 0.0f, 0.0f};
    }

    return (quat){
        .x = r / 2.0f,
        .y = m.m[i0][i1] - m.m[i1][i0] / (2.0f * r),
        .z = m.m[i2][i0] - m.m[i0][i2] / (2.0f * r),
        .w = m.m[i2][i1] - m.m[i1][i2] / (2.0f * r),
    };
}
#pragma endregion

#endif // TX_MATH_IMPL
