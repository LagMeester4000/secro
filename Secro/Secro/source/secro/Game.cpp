#include "Game.h"
#include <SFML/Graphics.hpp>
#include "framework/detail/PlainVectorMath.h"
#include "framework/player/PlayerCharacter.h"
#include "framework/DebugOptions.h"
#include "framework/GameplaySettings.h"
#include "gameplay/characters/CharacterDashette.h"
#include "gameplay/characters/CharacterDashetteP2.h"
#include "gameplay/characters/CharacterOki.h"
#include "gameplay/characters/CharacterPsycho.h"
#include "gameplay/level/BridgeLevel.h"

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
	level->addPlayer(std::make_shared<CharacterPsycho>());
	level->addPlayer(std::make_shared<CharacterPsycho>());

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
}

//TEMP
#include <SFML/Window.hpp>

void secro::Game::update(float deltaTime)
{
	inputManager->update();//netplay change

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

	//update the debug menu
	DebugOptions::update(deltaTime);
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
