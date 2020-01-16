#pragma once
#include "Vector2.h"

namespace secro {
	struct CollisionResult {
		Vector2 point;
		Vector2 normal;
		bool hit = false;
		
		//alpha value of when the hit happened
		sfloat timeOfHit;
	};
}