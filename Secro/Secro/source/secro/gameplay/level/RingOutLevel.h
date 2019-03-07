#pragma once
#include "secro/framework/level/Level.h"

namespace sf {
	class Font;
}

namespace secro {
	class RingOutLevel : public Level {
	public:
		RingOutLevel(std::shared_ptr<InputManager> input);

		void init() override;

		void update(float deltaTime) override;

		void render(sf::RenderWindow& window) override;

		bool isGameOver() override;

	private:
		void renderScores(sf::RenderWindow& window);
		void renderScore(sf::RenderWindow& window, sf::Vector2f position, PlayerCharacter* player, int stocks);

	public: //gamemode rules
		b2Vec2 boundsMin, boundsMax;
		int stockAmount;

	private:
		sf::Font* uiFont;
		std::vector<int> playerStocks;
	};
}