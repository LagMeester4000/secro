#pragma once
#include <vector>
#include <memory>
#include "Rules.h"

namespace sf {
	class RenderWindow;
}

namespace secro {
	class PlayerCharacter;

	class PlayerManager {
		struct PlayerRecord {
			std::shared_ptr<PlayerCharacter> player;
			int stocks;
		};
	public:
		PlayerManager();

		//set the size of the stage
		void setStageSize(float width, float height);

		//add a player to the player pool
		void addPlayer(PlayerCharacter* player);

		//get a player with a certain index
		PlayerCharacter* getPlayer(int index);

		//update the players
		void update(float deltaTime);

		//render the players
		void render(sf::RenderWindow& window);

		//get the size of the players vector
		size_t size();

		//access the players vector
		const PlayerRecord& operator[](size_t index);

		//check if the game is done
		bool isGameDone();

		//get the winner of the game
		int getWinner();

		//reset the positions and damage of the players
		void resetGame();

	private:
		//players
		std::vector<PlayerRecord> players;

		//stage
		float stageWidth, stageHeight;

		//the rules of the match
		Rules rules;

	};
}