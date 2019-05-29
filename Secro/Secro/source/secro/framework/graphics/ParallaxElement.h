#pragma once
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>

namespace sf {
	class RenderWindow;
}

namespace secro {
	class Camera;

	class ParallaxElement {
	public:
		ParallaxElement();

		void update(const Camera& cam);

		void render(sf::RenderWindow& window);

	public:
		sf::Vector2f startPosition = { 0.f, 0.f };
		sf::Vector2f cameraFactor = { 0.5f, 0.5f };
		int width = 1;
		int height = 1;
		sf::Sprite sprite;

	private:
		sf::Vector2f currentPosition = { 0.f, 0.f };

	};
}