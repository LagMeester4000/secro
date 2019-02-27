#pragma once
#include <SFML/System/Vector2.hpp>
#include "secro/framework/detail/PlainVectorMath.h"

namespace secro {
	struct UITransform {
		sf::Vector2f position = { 0.f, 0.f };
		sf::Vector2f scale = { 1.f, 1.f };
		float rotation = 0.f;
	};

	UITransform makeTransform(const UITransform& parent, const UITransform& child);
}