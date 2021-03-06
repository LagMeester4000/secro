#pragma once
#include "secro/framework/level/Level.h"
#include "secro/framework/graphics/ParallaxElement.h"
#include <SFML/Graphics/Sprite.hpp>

namespace sf {
	class Font;
}

namespace secro {
	class RingOutLevel : public Level {
	public:
		RingOutLevel(std::shared_ptr<InputManager> input);

		void init() override;

		void update(float deltaTime) override;
		void updateSimulate(float deltaTime) override;

		void render(sf::RenderWindow& window) override;

		void reset() override;

		bool isGameOver() override;

		void netSerSave(RawSerializeBuffer& buff) override;
		void netSerLoad(RawSerializeBuffer& buff) override;

	private: //graphics
		void renderScores(sf::RenderWindow& window);
		void renderScore(sf::RenderWindow& window, sf::Vector2f position, PlayerCharacter* player, int stocks);

		sf::Vector2f tilePos(sf::Vector2f begin, sf::Vector2i offset);
		void renderTile(sf::RenderWindow& window, sf::Vector2f position, sf::Vector2i size, sf::Vector2i tileIndex);

	private: 
		sf::Sprite groundTiles;
		ParallaxElement background;

	public: //gamemode rules
		Vector2 boundsMin, boundsMax;
		int stockAmount;

	private:
		sf::Font* uiFont;
		std::vector<int> playerStocks;
	};
}