#pragma once
#include "Vector2.h"

namespace secro {
	struct RayCastResult {
		Vector2 position;
		Vector2 normal;
		sfloat alpha;
		bool hit;
	};
}