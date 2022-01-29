#ifndef _UTILS_H
#define _UTILS_H
#include <stdio.h>
#include "raylib.h"


Vector2
AddVector2(const Vector2 a, const Vector2 b)
{
    Vector2 re = { 0 };
    re.x = a.x +b.x;
    re.y = a.y +b.y;
    return re;
}

Vector2
SubVector2(const Vector2 a, const Vector2 b)
{
    Vector2 re = { 0 };
    re.x = a.x -b.x;
    re.y = a.y -b.y;
    return re;
}

static void
DrawDebugText(unsigned int index, const char *fmt, ...)
{
    static const unsigned int debug_height_min = 28;
    static const unsigned int font_size = 26;
    char text[64] = "";
    va_list ap;
    va_start(ap, fmt); {
        vsnprintf(text, 64, fmt, ap);
    } va_end(ap);
    DrawText(text, 10, index*font_size +debug_height_min, font_size, LIME);
}

bool
collide(Vector3 a_pos, Vector3 a_size, Vector3 b_pos, Vector3 b_size)
{
    Vector3 a_lim = a_pos,
            b_lim = b_pos;
    a_lim.x += a_size.x;    b_lim.x += b_size.x;
    a_lim.y += a_size.y;    b_lim.y += b_size.y;
    a_lim.z += a_size.z;    b_lim.z += b_size.z;
    return CheckCollisionBoxes((BoundingBox){ a_pos, a_lim }, (BoundingBox){ b_pos, b_lim });
}
#endif // _UTILS_H
