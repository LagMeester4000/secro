#pragma once
#include "PhysicsDef.h"

namespace secro {
	struct Vector2 {
		sfloat x = 0.f;
		sfloat y = 0.f;
	};

	Vector2 operator+(const Vector2& lhs, const Vector2& rhs);
	Vector2& operator+=(Vector2& lhs, const Vector2& rhs);
	Vector2 operator-(const Vector2& lhs, const Vector2& rhs);
	Vector2& operator-=(Vector2& lhs, const Vector2& rhs);
	Vector2 operator*(const Vector2& lhs, const Vector2& rhs);
	Vector2 operator*(const Vector2& lhs, sfloat rhs);
	Vector2& operator*=(Vector2& lhs, const Vector2& rhs);
	Vector2& operator*=(Vector2& lhs, sfloat rhs);
	Vector2 operator/(const Vector2& lhs, const Vector2& rhs);
	Vector2 operator/(const Vector2& lhs, sfloat rhs);
	Vector2& operator/=(Vector2& lhs, const Vector2& rhs);
	Vector2& operator/=(Vector2& lhs, sfloat rhs);
	bool operator==(const Vector2& lhs, const Vector2& rhs);

	sfloat length(const Vector2& vec);
	Vector2 normalise(const Vector2& vec);
}