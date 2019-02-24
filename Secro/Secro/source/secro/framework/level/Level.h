#pragma once
#include <vector>
#include <memory>

namespace sf {
	class RenderWindow;
}

namespace secro {
	class PlayerCharacter;
	class Entity;

	class Level {
	public:

		virtual void init();
		virtual void update(float deltaTime);
		virtual void render(sf::RenderWindow& window);
		virtual void addPlayer(std::shared_ptr<PlayerCharacter> newPlayer);

		virtual bool isGameOver();
		virtual std::shared_ptr<PlayerCharacter> getWinningPlayer();

	private:


	};
}