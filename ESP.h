#pragma once
#include <string>
#include <vector>
#include "Maths.h"


class ESP
{
public:
	static bool WorldToScreen(Vec3 WorldPos, float ScreenWidth, float ScreenHeight, Matrix4x4F mat,Vec2& result);
private:

};

