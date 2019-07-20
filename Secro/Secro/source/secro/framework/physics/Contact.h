#pragma once
#include "CollisionResult.h"
#include "secro/framework/detail/Handle.h"

namespace secro {
	class Collider;

	struct Contact {
		Handle<Collider> colliderA, colliderB;
		CollisionResult lastCollision;
	};
}