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
		bool inFrontOfCharacter = true;

		AnimatedSprite animation;
		bool useAnimation = true;

		float opacityOverTime = 0.f;
		float opacity = 100.f;

		float scaleOverTime = 0.f;
		sf::Vector2f scale = { 0.05f, 0.05f };
	};
}