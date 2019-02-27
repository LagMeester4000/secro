#pragma once
#include "secro/framework/physics/PhysicsManager.h"
#include "secro/framework/collision/HitboxManager.h"
#include "secro/gameplay/Camera.h"
#include <vector>
#include <memory>

namespace sf {
	class RenderWindow;
}

namespace secro {
	class InputManager;
	class PlayerCharacter;
	class Entity;

	//level type that can have its own win condition
	class Level {
	public:
		Level(std::shared_ptr<InputManager> input);

		//start the level
		//called after all players have been added
		virtual void init();

		//update physics, camera, and players
		//shoul be called at the end of a child function
		virtual void update(float deltaTime);

		//render the stage, players, ui, and anything else
		virtual void render(sf::RenderWindow& window);

		//adds a player to the level
		//called from the game/menu select
		virtual void addPlayer(std::shared_ptr<PlayerCharacter> newPlayer);

		//returns if the game is over
		//is virtual because each level can have their own win condition
		virtual bool isGameOver();

		//returns the player who won
		virtual std::shared_ptr<PlayerCharacter> getWinningPlayer();

	private:
		PhysicsManager physicsManager;
		std::shared_ptr<InputManager> inputManager;
		HitboxManager hitboxManager;
		Camera camera;

		//entities
		std::vector<std::shared_ptr<PlayerCharacter>> players;
	};
}