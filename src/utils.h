#ifndef _UTILS_H
#define _UTILS_H
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
DrawDebugText(const char *text, unsigned int index)
{
    static const unsigned int debug_height_min = 28;
    static const unsigned int font_size = 16;
    DrawText(text, 10, index*font_size +debug_height_min, font_size, DARKGREEN);
}

#endif // _UTILS_H
