#pragma once
#include <Box2D/Common/b2Math.h>
#include "collision/FacingDirection.h"

namespace secro {
	class Hitbox;

	//entity for gameplay purposes
	class Entity {
	public:
		virtual ~Entity() {}

		virtual void init() {}
		virtual void update(float deltaTime) {}
		virtual void render(/**/) {}

		virtual b2Vec2 getPosition() { return { 0.f, 0.f }; }
		virtual b2Vec2 getScale() { return { 1.f, 1.f }; }
		virtual FacingDirection getFacingDirection() { return FacingDirection::Right; }
		virtual bool canBeDestroyed() { return false; }
		virtual void onReceiveHit(const Hitbox& hit, Entity* other) {}
		virtual void onSuccessfulHit(const Hitbox& hit, Entity* other) {}

		//get the last hit ID
		int& getLastHitId();

	protected:
		//the id the player got hit by last
		int lastHitId = -1;

		//
	};
}