#pragma once
#include "Vector2.h"
#include "ColliderProfile.h"

namespace secro {
	//Collider base class
	//extended by other classes with use of "CollisionResult collide(...)" function
	class Collider {
	public:
		Collider();

		void update(sfloat deltaTime);

		Vector2 getPosition() const;
		void setPosition(Vector2 vec);
		void updatePosition(sfloat deltaTime);
		
		Vector2 getVelocity() const;
		void setVelocity(Vector2 vec);

		ColliderProfile getProfile() const;
		void setProfile(ColliderProfile prof);

	protected:
		Vector2 position;
		Vector2 velocity;
		ColliderProfile profile;
	};
}