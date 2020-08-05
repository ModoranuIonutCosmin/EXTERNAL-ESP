#include "pch.h"
#include "ESP.h"
#include <TlHelp32.h>


bool ESP::WorldToScreen(Vec3 pos, float ScreenWidth, float ScreenHeight, Matrix4x4F mat, Vec2& result)
{
    float x, y, w;
    x = pos.x * mat[0][0] + pos.y *     mat[0][1] + pos.z *  mat[0][2] +  mat[0][3];
    y = pos.x * mat[1][0] + pos.y *     mat[1][1] + pos.z *  mat[1][2] +  mat[1][3];
    w = pos.x * mat[3][0] + pos.y *    mat[3][1] + pos.z * mat[3][2] + mat[3][3];


    if (w < 0.01f)
        return false;

    x *= (float)1 / w;
    y *= (float)1 / w;

    result.x = (ScreenWidth / 2 * x) + (x + ScreenWidth / 2);
    result.y = -(ScreenHeight / 2 * y) + (y + ScreenHeight / 2);
    return true;
}
