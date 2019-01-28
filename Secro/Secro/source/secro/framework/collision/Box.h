#pragma once
#include <Box2D/Common/b2Math.h>

namespace secro {
	//AABB box
	struct Box {
		b2Vec2 p1, p2;
	};
}