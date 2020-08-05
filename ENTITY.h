#pragma once
#include "ESP.h"
#include <vector>
class ENTITY
{
public:
	int HP;
	int AP;
	int Team;
	DWORD baseAddy;
	std::string name;
	Vec3 PosFeet,PosHead;
	Vec2 ScreenPosFeet,ScreenPosHead;
	bool isDormant;
	ENTITY();
	~ENTITY();
};

