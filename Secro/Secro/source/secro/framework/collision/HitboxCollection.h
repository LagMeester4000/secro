#pragma once
#include <vector>
#include <memory>
#include "Hitbox.h"
#include "HitboxChange.h"
#include "HitResult.h"

namespace sf {
	class RenderWindow;
}

namespace secro {
	class Hitbox;
	class PlayerCharacter;
	class Entity;
	struct FrameData;

	class HitboxCollection {
	public:
		HitboxCollection(Entity* owner, FrameData& framedata, bool isHitbox);

		//update the bounding volume if needed
		void update();

		//check for collision
		HitResult collide(HitboxCollection& other);

		//render the hitboxes
		void debugRender(sf::RenderWindow& window);

		//change/update the hitboxes
		void changeHitboxes(std::vector<HitboxChange>& changes);
		void changeHitboxes(HitboxChange& changes);

		//get the absolute volume of the hitbox collection
		Box getVolume();

		//mark the hitbox as deleted
		void destroy();

		//check if the hitbox should be deleted
		bool shouldDestroy();

		//get the owner entity of the hitbox
		Entity* getOwner();

		//get the hit Id
		int getHitId();

	private:
		//update the bounding volume
		void updateRelativeBox();

		static b2Vec2 scale(Entity* entity);

	private:
		//reference to owner character
		//if nullptr, the hitbox doesn't have an owner
		Entity* owner;
		
		//oredered list of hitboxes
		std::vector<std::shared_ptr<Hitbox>> hitboxes;

		//dirty flag to check if the hitboxes have been updated
		//might not be needed
		bool updatedFlag;

		//relative bounding volume for all hitboxes
		Box relativeBox;

		//if the hitbox should be deleted
		bool shouldDelete = false;
		
		//to determine if should be hit
		int hitId;

	private:
		static int hitIdCounter;

		int makeHitId();
	};
}