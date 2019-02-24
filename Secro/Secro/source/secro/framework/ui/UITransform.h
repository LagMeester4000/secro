#pragma once
#include <SFML/System/Vector2.hpp>
#include "secro/framework/detail/PlainVectorMath.h"

namespace secro {
	struct UITransform {
		sf::Vector2f position = { 0.f, 0.f };
		sf::Vector2f scale = { 1.f, 1.f };
		float rotation = 0.f;
	};

	UITransform transform(const UITransform& parent, const UITransform& child)
	{
		UITransform ret;
		ret.position = parent.position + mul(child.position, parent.scale); //change for rotation
		ret.scale = mul(parent.scale, child.scale);
		ret.rotation = parent.rotation + child.rotation;
	}
}