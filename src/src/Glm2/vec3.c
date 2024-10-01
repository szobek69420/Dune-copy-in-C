#include "vec3.h"

#include <math.h>
#include <stdio.h>

Vec3 vec3_create(float szam)
{
    return (Vec3){ szam, szam, szam };
}
Vec3 vec3_create2(float x, float y, float z)
{
    return (Vec3){ x, y, z };
}

Vec3 vec3_sum(Vec3 egy, Vec3 katto)
{
    return (Vec3){ egy.x + katto.x, egy.y + katto.y, egy.z + katto.z }; 
}
Vec3 vec3_subtract(Vec3 egy, Vec3 katto)
{
    return (Vec3){ egy.x - katto.x, egy.y - katto.y, egy.z - katto.z };
}
float vec3_dot(Vec3 egy, Vec3 katto)
{
    return egy.x * katto.x + egy.y * katto.y + egy.z * katto.z;
}
Vec3 vec3_cross(Vec3 egy, Vec3 katto)
{
    return vec3_create2(egy.y * katto.z - egy.z * katto.y, egy.z * katto.x - egy.x * katto.z, egy.x * katto.y - egy.y * katto.x);
}

float vec3_sqrMagnitude(Vec3 vec)
{
    return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
}

float vec3_magnitude(Vec3 vec)
{
    return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

Vec3 vec3_normalize(Vec3 vec)
{
    float length = vec3_magnitude(vec);
    return (Vec3){ vec.x / length, vec.y / length, vec.z / length };
}
Vec3 vec3_scale(Vec3 vec, float scalar)
{
    return (Vec3){ vec.x * scalar, vec.y * scalar, vec.z * scalar };
}

struct Vec3 vec3_lerp(struct Vec3 a, struct Vec3 b, float i)
{
    Vec3 result = a;
    result.x += (b.x - a.x) * i;
    result.y += (b.y - a.y) * i;
    result.z += (b.z - a.z) * i;
    return result;
}

struct Vec3 vec3_reflect(struct Vec3 vec, struct Vec3 normal)
{
    normal = vec3_normalize(normal);
    vec = vec3_subtract(vec, vec3_scale(normal, 2 * vec3_dot(vec, normal)));
    return vec;
}

void vec3_print(Vec3* vec) 
{
    printf("(%.2f; %.2f; %.2f)\n", vec->x, vec->y, vec->z);
}
