#pragma once
#include "PhysicsDef.h"
#include "Vector2.h"

namespace secro {
	class BoxCollider;

	class RayCastCallback {
	public:
		RayCastCallback() {}

		//function called on ray hit
		//returned value determines if the ray continues
		//true = continue
		//false = stop
		virtual bool onHit(BoxCollider& collider, Vector2 point, Vector2 normal, sfloat depth) = 0;
	};
}