#pragma once
#include <memory>
#include "framework/physics/PhysicsManager.h"
#include "framework/input/InputManager.h"
#include "framework/collision/HitboxManager.h"
#include "gameplay/PlayerManager.h"
#include "gameplay/Camera.h"
#include "framework/input/InputTestBot.h"
#include "framework/ui/UIManager.h"

//TEMP
#include "framework/collision/CollisionRenderer.h"
#include <SFML/Graphics/Font.hpp>

namespace sf {
	class RenderWindow;
}

namespace secro {
	class PlayerCharacter;

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

	private:
		void renderScores(sf::RenderWindow& window);
		void renderScore(sf::RenderWindow& window, sf::Vector2f position, PlayerCharacter* player, int stocks);

	private:
		PhysicsManager physicsManager;
		std::shared_ptr<InputManager> inputManager;
		PlayerManager playerManager;
		HitboxManager hitboxManager;
		Camera camera;

	private: //debug
		InputTestBot* inputBot;

	private: //TEMP
		CollisionRenderer c;
		sf::Font uiFont;

	private: //death animation
		void updateDeath(float deltaTime);
		float dtScalar;
		float deathSpeed;
		float deathDuration;
		float deathTimer;

	private: //ui
		UIManager ui;
	};
}