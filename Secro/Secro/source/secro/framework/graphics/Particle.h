#pragma once
#include "AnimatedSprite.h"

namespace sf {
	class RenderWindow;
}

namespace secro {
	class Particle {
	public:
		Particle();

		void update(float deltaTime);

		void render(sf::RenderWindow& window);

		bool canBeDeleted();

	public:
		AnimatedSprite animation;
	};
}