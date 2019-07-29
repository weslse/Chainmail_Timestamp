#pragma once
#include "DefTypes.h"

class Vec3
{
public:
	float x;
	float y;
	float z;

public:
	Vec3() = default;
	Vec3(const float px, const float py, const float pz);
	Vec3(const Node& n);
	Vec3(const Position& p);
	Vec3(float* arr);
	~Vec3() = default;

public:
	Vec3 operator+(const Vec3& v);
	Vec3 operator-(const Vec3& v);
	Vec3 operator*(const float c);
	Vec3 operator=(const Node& n);
	Vec3 operator=(const Position& n);
	Vec3& operator+=(const Vec3& v);
	Vec3& operator-=(const Vec3& v);
	Vec3& operator*=(const float c);
	
	const float getLength();
	const float dst();
	void normailze();
};

