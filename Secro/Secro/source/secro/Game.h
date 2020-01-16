#pragma once
#include <memory>
#include "framework/physics/SecroPhysicsManager.h"
#include "framework/input/InputManager.h"
#include "framework/collision/HitboxManager.h"
#include "gameplay/PlayerManager.h"
#include "gameplay/Camera.h"
#include "framework/input/InputTestBot.h"
#include "framework/ui/UIManager.h"
#include "gameplay/level/RingOutLevel.h"
#include "ShobuNetwork/Network.h"
#include "secro/netplay/RawSerializeBuffer.h"
#include "KraNet/KraNetSession.h"
#include "netplay/NetworkDebugUI.h"

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
		void update(float deltaTime, bool shouldUpdateInput = true);
		void simulateUpdate(float deltaTime, bool duringRollback = false);
		void updateWithInput(float deltaTime, int localInput, int otherInput, bool duringRollback = false);

		//render the game
		void render(sf::RenderWindow& window);

	public:
		//start a level
		void startLevel(std::shared_ptr<Level> level, int amountOfPlayers);

	private:
		std::shared_ptr<InputManager> inputManager;
		std::shared_ptr<Level> level;

	public: //TEMP
		CollisionRenderer c;
		sf::Font uiFont;
		InputTestBot* inputBot;

	private: //ui
		UIManager ui;

		//gamestate
		GameState gameState;

	private: //netcode
		ShobuNetwork network;
		kra::net::KraNetSession kraNet;
		RawSerializeBuffer stateBuffer;
		NetworkDebugUI networkUI;

	public:
		void netStateSave();
		void netStateLoad();
		int netStateHash();

		static void staticNetStateUpdate(void* ex, KraNetInput p1, KraNetInput p2);
		static void staticNetStateSimulate(void* ex, KraNetInput p1, KraNetInput p2);
		static void staticNetStateSave(void* ex);
		static void staticNetStateLoad(void* ex);
	};
}