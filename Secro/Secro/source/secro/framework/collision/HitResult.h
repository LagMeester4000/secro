#pragma once
#include <memory>
#include <vector>

namespace secro {
	class Hitbox;

	//result returned when two hitboxcollections collide
	struct HitResult {
		//if at least one of the hitboxes have hit the hurtboxes
		bool hasHit;

		//hitboxes that have hit the hurtboxes of the other entity
		//ordered by priority
		std::vector<std::shared_ptr<Hitbox>> hits;

		//hurtboxes of the other entity that have been hit by the hitboxes
		//ordered by priority
		std::vector<std::shared_ptr<Hitbox>> hurts;
	};
}