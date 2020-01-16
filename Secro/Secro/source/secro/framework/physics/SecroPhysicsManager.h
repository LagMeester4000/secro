#pragma once
#include "Vector2.h"
#include "RayCastCallback.h"
#include "BoxCollider.h"
#include "CollisionResult.h"
#include "secro/netplay/RawSerializeBuffer.h"
#include <vector>

namespace sf {
	class RenderWindow;
}

namespace secro {
	class BoxCollider;

	struct PhysicsHandle {
		int handle = -1;
	};

	class SecroPhysicsManager {
	public:
		SecroPhysicsManager();

		//update the whol physics world
		void update(sfloat deltaTime);

		//render the physics boxes
		void debugRender(sf::RenderWindow& window);

		//get a collider from the physics manager
		BoxCollider& getCollider(PhysicsHandle handle);

		//add a collider to the 
		PhysicsHandle addCollider(const BoxCollider& collider);
		PhysicsHandle addPlayerCollider();

		//cast a ray
		//not really needed
		void rayCast(Vector2 point1, Vector2 point2, RayCastCallback* callback);

		//try to push a collider down
		bool pushDown(PhysicsHandle handle, sfloat distance);

	private:
		CollisionResult updateSide(size_t i, sfloat deltaTime, bool doVertical);

	public: //networking
		void netSerSave(RawSerializeBuffer& buff);
		void netSerLoad(RawSerializeBuffer& buff);

	private:
		std::vector<BoxCollider> colliders;
	};
}