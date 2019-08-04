#include "Vec3.h"
#include <cmath>


Vec3::Vec3(const float px, const float py, const float pz)
{
	x = px;
	y = py;
	z = pz;
}

Vec3::Vec3(const Node & n)
{
	x = n.position.x;
	y = n.position.y;
	z = n.position.z;
}

Vec3::Vec3(const Position& p)
{
	x = p.x;
	y = p.y;
	z = p.z;
}

Vec3::Vec3(float * arr)
{
	x = arr[0];
	y = arr[1];
	z = arr[2];
}

Vec3 Vec3::operator+(const Vec3& v)
{
	return Vec3(x + v.x, y + v.y, z + v.z);
}

Vec3 Vec3::operator-(const Vec3& v)
{
	return Vec3(x - v.x, y - v.y, z - v.z);
}

Vec3 Vec3::operator*(const float c)
{
	return Vec3(x*c, y*c, z*c);
}

Vec3 Vec3::operator=(const Node& n)
{
	return Vec3(n);
}

Vec3 Vec3::operator=(const Position& p)
{
	return Vec3(p);
}

Vec3 & Vec3::operator+=(const Vec3 & v)
{
	x += v.x;
	y += v.y;
	z += v.z;

	return (*this);
}

Vec3 & Vec3::operator-=(const Vec3 & v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;

	return (*this);
}

Vec3 & Vec3::operator*=(const float c)
{
	x *= c;
	y *= c;
	z *= c;

	return (*this);
}

const float Vec3::getLength()
{
	return sqrtf(x * x + y * y + z * z);
}

const float Vec3::dst()
{
	return getLength();
}

void Vec3::normalize()
{
	const float invLen = 1.f / getLength();
	(*this)*=invLen;
}
