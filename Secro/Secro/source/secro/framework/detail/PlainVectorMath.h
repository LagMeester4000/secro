#pragma once
#include <math.h>

#define PI 3.14159265359f

namespace secro {
	//it is assumed that the objects used in these functions have a public x and y member

	//subtract vec2 from vec1
	template<typename T>
	T min(const T& vec1, const T& vec2)
	{
		return { vec1.x - vec2.x, vec1.y - vec2.y };
	}

	//add vec2 to vec1
	template<typename T>
	T plus(const T& vec1, const T& vec2)
	{
		return { vec1.x + vec2.x, vec1.y + vec2.y };
	}

	//multiply vec1 by vec2
	template<typename T>
	T mul(const T& vec1, const T& vec2)
	{
		return { vec1.x * vec2.x, vec1.y * vec2.y };
	}

	//multiply vec1 by f
	template<typename T>
	T mul(const T& vec1, float f)
	{
		return { vec1.x * f, vec1.y * f };
	}

	//div vec1 by vec2
	template<typename T>
	T div(const T& vec1, const T& vec2)
	{
		return { vec1.x / vec2.x, vec1.y / vec2.y };
	}

	//div vec1 by f
	template<typename T>
	T div(const T& vec1, float f)
	{
		return { vec1.x / f, vec1.y / f };
	}

	template<typename T>
	float length(const T& vec)
	{
		return sqrtf(vec.x * vec.x + vec.y * vec.y);
	}

	template<typename T>
	float dot(const T& vec1, const T& vec2)
	{
		return vec1.x * vec2.x + vec1.y * vec2.y;
	}

	//get the normalise a vector
	template<typename T>
	T normalise(const T& vec)
	{
		float len = length(vec);
		return { vec.x / len, vec.y / len };
	}

	template<typename T>
	float distance(const T& vec1, const T& vec2)
	{
		return length(min(vec2, vec1));
	}

	template<typename T>
	T directionFromAngle(float angle)
	{
		angle = angle / 180.f * PI;
		T ret;
		ret.x = cos(angle);
		ret.y = sin(angle);
		return ret;
	}

	template<typename T>
	float angleFromDirection(const T& direction)
	{
		T directionR = normalise(direction);
		float ret = atan2(-directionR.x, directionR.y) / PI * 180.f + 90.f;
		return ret;
	}

	template<typename T>
	T adjustAngle(const T& vec, float angleChange)
	{
		float len = length(vec);
		float angle = angleFromDirection(vec);
		T ret = directionFromAngle<T>(angle + angleChange);
		ret = mul(ret, len);
		return ret;
	}

	template<typename T, typename U>
	T conv(U& other)
	{
		T ret;
		ret.x = other.x;
		ret.y = other.y;
		return ret;
	}

	template<typename T, typename U>
	T convNR(U other)
	{
		T ret;
		ret.x = other.x;
		ret.y = other.y;
		return ret;
	}
}