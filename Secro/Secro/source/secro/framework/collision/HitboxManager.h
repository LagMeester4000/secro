#pragma once
#include "HitboxCollection.h"

namespace secro {
	struct FrameData;

	//manager that holds all the hit and hurtboxes from the player used for damaging
	//this is separate from the physics
	class HitboxManager {
	public:
		HitboxManager();

		//check for collisions
		//delete marked hitboxes
		void update(float deltaTime);

		//render the hitboxes in the scene
		void render(sf::RenderWindow& window);

		//add a hitbox to the scene
		std::shared_ptr<HitboxCollection> addHitbox(Entity* owner, FrameData& framedata);

		//add a hurtbox to the scene
		//takes the first frame of the framedata as a constant hurtbox
		std::shared_ptr<HitboxCollection> addHurtbox(Entity* owner, FrameData& framedata);
		
	private:
		//delete all the mared hit and hurtboxes
		void deleteMarked();

	private:
		//all the hitboxes in the scene
		std::vector<std::shared_ptr<HitboxCollection>> hitboxes;

		//all the hurtboxes in the scene
		std::vector<std::shared_ptr<HitboxCollection>> hurtboxes;
	};
}