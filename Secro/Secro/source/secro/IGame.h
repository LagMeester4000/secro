#pragma once

namespace sf {
	class RenderWindow;
}

namespace secro {
	class IGame {
	public:
		virtual void update(float deltaTime) = 0;
		virtual void render(sf::RenderWindow& window) = 0;
	};
}