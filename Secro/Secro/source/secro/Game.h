#pragma once
#include <memory>
#include "framework/physics/PhysicsManager.h"
#include "framework/input/InputManager.h"
#include "framework/collision/HitboxManager.h"
#include "gameplay/PlayerManager.h"
#include "gameplay/Camera.h"
#include "framework/input/InputTestBot.h"
#include "framework/ui/UIManager.h"
#include "gameplay/level/RingOutLevel.h"

//TEMP
#include "framework/collision/CollisionRenderer.h"
#include <SFML/Graphics/Font.hpp>

namespace sf {
	class RenderWindow;
}

namespace secro {
	class PlayerCharacter;

	enum class GameState {
		MainMenu,
		Gameplay,
	};

	class Game {
	public:
		static std::shared_ptr<Game> createGame(std::shared_ptr<InputManager> input);

	private:
		Game(std::shared_ptr<InputManager> input);

	public:
		//update the game
		void update(float deltaTime);

		//render the game
		void render(sf::RenderWindow& window);

	public:
		//start a level
		void startLevel(std::shared_ptr<Level> level, int amountOfPlayers);

	private:
		std::shared_ptr<InputManager> inputManager;
		std::shared_ptr<Level> level;

	private: //TEMP
		CollisionRenderer c;
		sf::Font uiFont;

	private: //ui
		UIManager ui;

		//gamestate
		GameState gameState;
	};
}