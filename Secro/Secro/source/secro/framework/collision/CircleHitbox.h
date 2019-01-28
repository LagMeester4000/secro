#pragma once
#include "Hitbox.h"

namespace secro {
	class CircleHitbox : public Hitbox {
	public:
		//update the hitbox
		void update(HitboxChange& change) override;

		//render the hitbox
		void render(sf::RenderWindow& window, b2Vec2 position, b2Vec2 scale) override;

		//get the space that the hitbox uses
		Box getSpace() override;

		//check for collision with another hitbox
		bool collide(Hitbox& other, b2Vec2 otherPosition, b2Vec2 otherScale, b2Vec2 position, b2Vec2 scale) override;

	private:
		float radius;
	};
}