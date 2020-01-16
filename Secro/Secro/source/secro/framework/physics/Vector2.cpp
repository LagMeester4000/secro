#include "Vector2.h"
#include <cmath>

using namespace secro;

Vector2 secro::operator+(const Vector2 & lhs, const Vector2 & rhs)
{
	return Vector2{ lhs.x + rhs.x, lhs.y + rhs.y };
}

Vector2 & secro::operator+=(Vector2 & lhs, const Vector2 & rhs)
{
	lhs = Vector2{ lhs.x + rhs.x, lhs.y + rhs.y };
	return lhs;
}

Vector2 secro::operator-(const Vector2 & lhs, const Vector2 & rhs)
{
	return Vector2{ lhs.x - rhs.x, lhs.y - rhs.y };
}

Vector2 & secro::operator-=(Vector2 & lhs, const Vector2 & rhs)
{
	lhs = Vector2{ lhs.x - rhs.x, lhs.y - rhs.y };
	return lhs;
}

Vector2 secro::operator*(const Vector2 & lhs, const Vector2 & rhs)
{
	return Vector2{ lhs.x * rhs.x, lhs.y * rhs.y };
}

Vector2 secro::operator*(const Vector2 & lhs, sfloat rhs)
{
	return Vector2{ lhs.x * rhs, lhs.y * rhs };
}

Vector2 & secro::operator*=(Vector2 & lhs, const Vector2 & rhs)
{
	lhs = Vector2{ lhs.x * rhs.x, lhs.y * rhs.y };
	return lhs;
}

Vector2 & secro::operator*=(Vector2 & lhs, sfloat rhs)
{
	lhs = Vector2{ lhs.x * rhs, lhs.y * rhs };
	return lhs;
}

Vector2 secro::operator/(const Vector2 & lhs, const Vector2 & rhs)
{
	return Vector2{ lhs.x / rhs.x, lhs.y / rhs.y };
}

Vector2 secro::operator/(const Vector2 & lhs, sfloat rhs)
{
	return Vector2{ lhs.x / rhs, lhs.y / rhs };
}

Vector2 & secro::operator/=(Vector2 & lhs, const Vector2 & rhs)
{
	lhs = Vector2{ lhs.x / rhs.x, lhs.y / rhs.y };
	return lhs;
}

Vector2 & secro::operator/=(Vector2 & lhs, sfloat rhs)
{
	lhs = Vector2{ lhs.x / rhs, lhs.y / rhs };
	return lhs;
}

bool secro::operator==(const Vector2 & lhs, const Vector2 & rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

sfloat secro::length(const Vector2 & vec)
{
	return std::sqrt(vec.x * vec.x + vec.y * vec.y);
}

Vector2 secro::normalise(const Vector2 & vec)
{
	auto len = length(vec);
	return Vector2{ vec.x / len, vec.y / len };
}
