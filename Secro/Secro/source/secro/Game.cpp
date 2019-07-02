#include "Game.h"
#include <SFML/Graphics.hpp>
#include "framework/detail/PlainVectorMath.h"
#include "framework/player/PlayerCharacter.h"
#include "framework/DebugOptions.h"
#include "framework/GameplaySettings.h"
#include "framework/GameInstance.h"
#include "gameplay/characters/CharacterDashette.h"
#include "gameplay/characters/CharacterDashetteP2.h"
#include "gameplay/characters/CharacterOki.h"
#include "gameplay/characters/CharacterPsycho.h"
#include "gameplay/level/BridgeLevel.h"
#include "ShobuNetwork/Network.h"

//ui
#include "framework/ui/UIElement.h"
#include "framework/ui/UISimpleButton.h"
#include "framework/ui/UIMenu.h"

using namespace secro;

std::shared_ptr<Game> secro::Game::createGame(std::shared_ptr<InputManager> input)
{
	class make_shared_enabler : public Game 
	{
	public:
		make_shared_enabler(std::shared_ptr<InputManager> input)
			: Game(input)
		{}
	};

	return std::make_shared<make_shared_enabler>(input);
}

secro::Game::Game(std::shared_ptr<InputManager> input)
	: inputManager(input), ui(this)
{
	//set starting state
	gameState = GameState::MainMenu;

	//initialize input
	inputManager->init(2);

	//set up the stage
	auto newLevel = std::make_shared<RingOutLevel>(inputManager);
	level = newLevel;
	newLevel->boundsMax = { 20.f, 20.f };
	newLevel->boundsMin = { -20.f, -20.f };
	newLevel->stockAmount = 4;

	//set up the players
	//level->addPlayer(std::make_shared<CharacterDashette>());
	//level->addPlayer(std::make_shared<CharacterDashette>());

	//initialize the level
	level->init();


	DebugOptions::init();

	if (!uiFont.loadFromFile("LemonMilk.otf"))
		std::cout << "could not load font" << std::endl;

	//UI
	std::shared_ptr<UIMenu> uiBase = std::make_shared<UIMenu>();
	uiBase->visableWhenNotOnTop = false;
	std::shared_ptr<UIMenu> uiLevelSelect = std::make_shared<UIMenu>();
	ui.pushTopFrame(uiBase);
	sf::View view({ 0.f, 0.f }, { 1000.f, 1000.f });
	ui.setView(view);

	{//base ui
		//buttons
		std::shared_ptr<UISimpleButton> button1 = std::make_shared<UISimpleButton>();
		button1->font = uiFont;
		button1->text = "Play";
		button1->fontSize = 120.f;
		button1->transform.position = { -300.f, -150.f };
		button1->size = { 600.f, 200.f };
		auto& state = gameState;
		button1->onPressed = [=]()
		{
			ui.pushTopFrame(uiLevelSelect);
		};

		std::shared_ptr<UISimpleButton> button2 = std::make_shared<UISimpleButton>();
		button2->font = uiFont;
		button2->text = "Options";
		button2->fontSize = 100.f;
		button2->transform.position = { -300.f, 150.f };
		button2->size = { 600.f, 200.f };

		button1->downElement = &*button2;
		button2->upElement = &*button1;

		uiBase->addSelectable(button1);
		uiBase->addSelectable(button2);
	}

	{//level select
		//buttons
		std::shared_ptr<UISimpleButton> selectRingout = std::make_shared<UISimpleButton>();
		selectRingout->font = uiFont;
		selectRingout->text = "Ring Out";
		selectRingout->fontSize = 120.f;
		selectRingout->transform.position = { -300.f, -400.f };
		selectRingout->size = { 600.f, 200.f };
		auto& state = gameState;
		selectRingout->onPressed = [&]()
		{
			//set up the stage
			auto newLevel = std::make_shared<RingOutLevel>(inputManager);
			level = newLevel;
			newLevel->boundsMax = { 20.f, 20.f };
			newLevel->boundsMin = { -20.f, -20.f };
			newLevel->stockAmount = 4;
			startLevel(newLevel, 2);

			//hack the input
			//inputBot = new InputTestBot(&*level->players[1], *inputManager->getController(1), 1);
		};

		std::shared_ptr<UISimpleButton> selectBridge = std::make_shared<UISimpleButton>();
		selectBridge->font = uiFont;
		selectBridge->text = "Cross bridge";
		selectBridge->fontSize = 120.f;
		selectBridge->transform.position = { -300.f, -100.f };
		selectBridge->size = { 600.f, 200.f };
		selectBridge->onPressed = [&]()
		{
			//set up the stage
			auto newLevel = std::make_shared<BridgeLevel>(inputManager);
			level = newLevel;
			newLevel->bounds = { 10.f, 20.f };
			newLevel->pointAmount = 5;
			startLevel(newLevel, 2);
		};

		std::shared_ptr<UISimpleButton> backButton = std::make_shared<UISimpleButton>();
		backButton->font = uiFont;
		backButton->text = "Back";
		backButton->fontSize = 100.f;
		backButton->transform.position = { -300.f, 200.f };
		backButton->size = { 600.f, 200.f };
		backButton->onPressed = [&]()
		{
			ui.popTopFrame();
		};

		selectRingout->downElement = &*selectBridge;
		selectBridge->upElement = &*selectRingout;
		selectBridge->downElement = &*backButton;
		backButton->upElement = &*selectBridge;

		uiLevelSelect->addSelectable(selectRingout);
		uiLevelSelect->addSelectable(selectBridge);
		uiLevelSelect->addSelectable(backButton);
	}


	//online
	if (GameInstance::instance.isOnline)
	{
		//netplay!
		network.registerCallbacks(
			[](void* g, int me, int other)
		{
			Game* asGame = (Game*)g;
			asGame->updateWithInput(1.f / 60.f, me, other);
		},
			[](void* g)
		{
			Game* asGame = (Game*)g;
			asGame->netStateSave();
		},
			[](void* g)
		{
			Game* asGame = (Game*)g;
			asGame->netStateLoad();
		},
			[](void* g) -> int
		{
			//Game* asGame = (Game*)g;
			//return asGame->netStateHash();
			return 0;
		}, (void*)this);

		std::cout << "0 for host, 1 for client" << std::endl;
		int i;
		std::cin >> i;
		if (i == 0)
		{
			//host
			int delay = 3;
			std::cout << "set frames of input delay (default value is 3)" << std::endl;
			std::cin >> delay;
			std::cout << std::endl;

			network.setInputDelay(delay);
			network.initializeHost(31415);
			network.waitForClient();
		}
		else
		{
			//client
			std::string ip = "";
			std::cout << "enter IP address" << std::endl;
			std::cin >> ip;
			std::cout << std::endl;

			network.initializeClient(ip.c_str(), 31415);

			//connect
			network.connectToHost();

			int stop = 0;
		}
	}

	//set up the stage
	startLevel(newLevel, 2);
}

//TEMP
#include <SFML/Window.hpp>

void secro::Game::update(float deltaTime, bool shouldUpdateInput)
{
	//if (shouldUpdateInput)
	//{
	//	if (network.isHost())
	//		inputManager->update(); //netplay change
	//	else
	//		inputManager->updateNoInputs();
	//}

	if (GameInstance::instance.isOnline)
	{
		if (network.connected() && shouldUpdateInput)
		{
			//print ping
			std::cout << network.getPing() << std::endl;
			if (network.getPing() > 200)
			{
				using namespace std::chrono_literals;
				std::this_thread::sleep_for(30ms);
			}

			//if (network.isHost())
			//{
			auto r = inputManager->getController(0)->readInput();
			network.update(Controller::compressInput(r).input.raw);
			//}
			//else
			//{
			//	network.update(0);
			//}
		}
	}
	else
	{
		//offline
		inputManager->update();
		simulateUpdate(deltaTime);
		level->physicsManager.serReset();
	}

	//update the debug menu
	DebugOptions::update(deltaTime);
}

void secro::Game::simulateUpdate(float deltaTime)
{
	switch (gameState)
	{
	case GameState::MainMenu:
	{
		ui.update(deltaTime);
		ui.handleInput(*inputManager->getController(0));
	} break;
	case GameState::Gameplay:
	{
		level->update(deltaTime);
		if (level->isGameOver())
		{
			gameState = GameState::MainMenu;
		}
		if (level->isGameOver())
		{
			gameState = GameState::MainMenu;
		}
	} break;
	}
}

void secro::Game::updateWithInput(float deltaTime, int localInput, int otherInput)
{
	CompressedInput c0, c1;

	if (network.isHost())
	{
		c0.input.raw = localInput;
		c1.input.raw = otherInput;
	}
	else
	{
		c1.input.raw = localInput;
		c0.input.raw = otherInput;
	}
	Controller::Input i0 = Controller::uncompressInput(c0);
	Controller::Input i1 = Controller::uncompressInput(c1);

	inputManager->getController(0)->manualUpdate(i0);
	inputManager->getController(1)->manualUpdate(i1);

	simulateUpdate(deltaTime);
}

void secro::Game::render(sf::RenderWindow & window)
{
	switch (gameState)
	{
	case GameState::MainMenu:
	{
		//render ui
		ui.render(window);
	} break;
	case GameState::Gameplay:
	{
		//render gameplay
		level->render(window);

		if (GameInstance::instance.isOnline)
		{
			//capture the view and make a new one
			auto resetView = window.getView();
			sf::View view(sf::FloatRect{ 0.f, 0.f, 1920.f, 1080.f });
			window.setView(view);

			//draw UI
			std::string pingStr = "ping: ";
			pingStr += std::to_string(network.getPing());
			sf::Text t(pingStr.c_str(), uiFont);

			//reset the view
			window.setView(resetView);
		}
	} break;
	}


	//TEMP
	c.renderFrameDataEditor(window);

	//render the debug menu
	DebugOptions::render(window);

	//render the gameplay settings
	GameplaySettings::render();

	//render the input bot
	//inputBot->render();
}

void secro::Game::startLevel(std::shared_ptr<Level> level, int amountOfPlayers)
{
	for (int i = 0; i < amountOfPlayers; ++i)
	{
		level->addPlayer(std::make_shared<CharacterDashette>());
	}
	//level->addPlayer(std::make_shared<CharacterPsycho>());
	//level->addPlayer(std::make_shared<CharacterDashette>());
	level->init();
	gameState = GameState::Gameplay;
}

void secro::Game::netStateSave()
{
	stateBuffer.reset();
	inputManager->netSerSave(stateBuffer);
	level->physicsManager.netSerSave(stateBuffer);
	level->hitboxManager.netSerSave(stateBuffer);
	level->netSerSave(stateBuffer);

	for (auto& it : level->players)
	{
		it->netSerSave(stateBuffer);
	}
}

void secro::Game::netStateLoad()
{
	stateBuffer.reset();
	inputManager->netSerLoad(stateBuffer);
	level->physicsManager.netSerLoad(stateBuffer);
	level->hitboxManager.netSerLoad(stateBuffer);
	level->netSerLoad(stateBuffer);

	for (auto& it : level->players)
	{
		it->netSerLoad(stateBuffer);
	}
}

int secro::Game::netStateHash()
{
	return stateBuffer.hash();
}
