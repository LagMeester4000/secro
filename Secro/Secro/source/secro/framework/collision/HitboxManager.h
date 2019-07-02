#pragma once
#include "HitboxCollection.h"
#include "HitboxId.h"

namespace secro {
	struct FrameData;
	class Level;
	class RawSerializeBuffer;

	//manager that holds all the hit and hurtboxes from the player used for damaging
	//this is separate from the physics
	class HitboxManager {
	public:
		HitboxManager();

		//check for collisions
		//delete marked hitboxes
		void update(float deltaTime, Level& level);

		//render the hitboxes in the scene
		void render(sf::RenderWindow& window);

		//add a hitbox to the scene
		HitboxId addHitbox(Entity* owner, FrameData& framedata);

		//add a hurtbox to the scene
		//takes the first frame of the framedata as a constant hurtbox
		HitboxId addHurtbox(Entity* owner, FrameData& framedata);

		//get a hitbox from a handle
		HitboxCollection* getHitbox(HitboxId id);

		//get a hurtbox from a handle
		HitboxCollection* getHurtbox(HitboxId id);

		//net serialize save
		void netSerSave(RawSerializeBuffer& buff);

		//net serialize load
		void netSerLoad(RawSerializeBuffer& buff);

	private:
		//clear all the hitboxes
		void clearHitboxes();

		//returns the amount of hitboxes that are not marked as delete
		size_t amountOfActiveHitboxes(std::vector<HitboxCollection>& hitboxes);

	private:
		//all the hitboxes in the scene
		std::vector<HitboxCollection> hitboxes;

		//all the hurtboxes in the scene
		std::vector<HitboxCollection> hurtboxes;
	};
}