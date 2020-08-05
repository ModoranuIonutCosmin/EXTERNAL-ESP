#pragma once
struct Matrix4x4F // view matrix
{
	float* operator[](int index)
	{
		return m[index];
	}
	float m[4][4];
};

struct Matrix3x4F // bone matrix
{
	float* operator[](int index)
	{
		return m[index];
	}
	float m[3][4];
};
struct Vec2
{
	float x, y;
	Vec2()
	{
		x = y = 0;
	}
	Vec2(float x, float y)
	{
		this->x = x;
		this->y = y;
	}
	Vec2 operator+ (const Vec2& other)
	{
		Vec2 dst;
		dst.x = this->x + other.x;
		dst.y = this->y + other.y;
		return dst;
	}
	Vec2 operator- (const Vec2& other)
	{
		Vec2 dst;
		dst.x = this->x - other.x;
		dst.y = this->y - other.y;
		return dst;
	}
};

struct Vec3
{
	float x, y, z;
	Vec3()
	{
		x = y = z = 0;
	}
	Vec3(float xx, float yy, float zz)
	{
		x = xx;
		y = yy;
		z = zz;
	}
	Vec3 operator+ (const Vec3& other)
	{
		Vec3 dst;
		dst.x = this->x + other.x;
		dst.y = this->y + other.y;
		dst.z = this->z + other.z;
		return dst;
	}
	Vec3 operator- (const Vec3& other)
	{
		Vec3 dst;
		dst.x = this->x - other.x;
		dst.y = this->y - other.y;
		dst.z = this->z - other.z;
		return dst;
	}
};
class Maths
{
	void doStuff();
};