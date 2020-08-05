#include "pch.h"
#include "ENTITY.h"

ENTITY::ENTITY()
{
	HP = AP = baseAddy = Team = 0;
	name = "";
	isDormant = false;
}

ENTITY::~ENTITY()
{

}
