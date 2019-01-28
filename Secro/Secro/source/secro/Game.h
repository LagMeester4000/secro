#pragma once
#include <memory>
#include "framework/physics/PhysicsManager.h"
#include "framework/input/InputManager.h"
#include "framework/collision/HitboxManager.h"

//TEMP
#include "framework/collision/CollisionRenderer.h"

namespace sf {
	class RenderWindow;
}

namespace secro {
	class PlayerCharacter;

	class Game {
	public:
		static std::shared_ptr<Game> createGame();

	private:
		Game();

	public:
		//update the game
		void update(float deltaTime);

		//render the game
		void render(sf::RenderWindow& window);

	private:
		PhysicsManager physicsManager;
		InputManager inputManager;
		PlayerCharacter* player;
		PlayerCharacter* dummyPlayer;
		HitboxManager hitboxManager;

	private: //TEMP
		CollisionRenderer c;

	};
}