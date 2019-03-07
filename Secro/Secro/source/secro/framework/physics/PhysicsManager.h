#pragma once
#include <vector>
#include <Box2D/Box2D.h>
#include "PlayerCollision.h"
#include "StageCollision.h"

namespace sf {
	class RenderWindow;
}

namespace secro {
	class PhysicsManager{
	public:
		PhysicsManager();
		~PhysicsManager();

		//udpate the physics world
		void update(float deltaTime);

		//render the colliders
		//for debugging purposes
		void debugRender(sf::RenderWindow& window);

		//add a stage collider
		void addStageCollider(b2Vec2 position, b2Vec2 size, bool isPlatform = false);

		//make a player body
		b2Body* makePlayerBody();

		//get the physics body of a player
		b2Body* getPlayerBody(int index);
		
	private:
		std::shared_ptr<b2World> world;

		std::vector<PlayerCollision> playersColliders;
		std::vector<StageCollision> stageColliders;
	};
}