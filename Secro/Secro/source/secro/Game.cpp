#include "Game.h"
#include <SFML/Graphics.hpp>
#include "framework/detail/PlainVectorMath.h"
#include "framework/player/PlayerCharacter.h"
#include "framework/DebugOptions.h"
#include "framework/GameplaySettings.h"
#include "gameplay/characters/CharacterDashette.h"
#include "gameplay/characters/CharacterDashetteP2.h"

using namespace secro;

std::shared_ptr<Game> secro::Game::createGame()
{
	class make_shared_enabler : public Game {};

	return std::make_shared<make_shared_enabler>();
}

secro::Game::Game()
{
	inputManager.init(2);

	//set up the players
	playerManager.addPlayer(new CharacterDashette(&hitboxManager, physicsManager.getPlayerBody(0), inputManager.getController(0)));
	auto* secondPlayer = new PlayerCharacter(&hitboxManager, physicsManager.getPlayerBody(1), inputManager.getController(1));
	playerManager.addPlayer(secondPlayer);

	//hack the input
	inputBot = new InputTestBot(secondPlayer, *inputManager.getController(1), 1);

	//set up the stage
	playerManager.setStageSize(20.f, 20.f);

	DebugOptions::init();

	if (!uiFont.loadFromFile("LemonMilk.otf"))
		std::cout << "could not load font" << std::endl;

	//set death animation values
	deathTimer = -1.f;
	deathSpeed = 0.5f;
	deathDuration = 3.f;
	dtScalar = 1.f;
}

//TEMP
#include <SFML/Window.hpp>

void secro::Game::update(float deltaTime)
{
	updateDeath(deltaTime);
	inputManager.update();
	playerManager.update(deltaTime * dtScalar);
	physicsManager.update(deltaTime * dtScalar);
	hitboxManager.update(deltaTime * dtScalar);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
	{
		physicsManager.getPlayerBody(0)->SetTransform(b2Vec2(-5.f, -2.f), 0.f);
		physicsManager.getPlayerBody(1)->SetTransform(b2Vec2(5.f, -2.f), 0.f);
	}
	
	//update the camera
	std::vector<PlayerCharacter*> players { playerManager.getPlayer(0), playerManager.getPlayer(1) };
	camera.update(deltaTime * dtScalar, players);

	//update the debug menu
	DebugOptions::update(deltaTime);
}

void secro::Game::render(sf::RenderWindow & window)
{
	physicsManager.debugRender(window);
	playerManager.render(window);

	//TEMP
	c.renderFrameDataEditor(window);
	hitboxManager.render(window);

	//camera
	camera.render(window);

	//render the debug menu
	DebugOptions::render(window);

	//render the gameplay settings
	GameplaySettings::render();

	//render the input bot
	inputBot->render();

	//render the score UI
	renderScores(window);
}

void secro::Game::renderScores(sf::RenderWindow & window)
{
	//capture the view and make a new one
	auto resetView = window.getView();
	sf::View view(sf::FloatRect{ 0.f, 0.f, 1920.f, 1080.f });
	window.setView(view);

	for (size_t i = 0; i < playerManager.size(); ++i)
	{
		auto& p = playerManager[i];
		renderScore(window, { 640.f + 400.f * (float)i, 900.f }, &*p.player, p.stocks);
	}

	//reset the view
	window.setView(resetView);
}

void secro::Game::renderScore(sf::RenderWindow & window, sf::Vector2f position, PlayerCharacter * player, int stocks)
{
	float textSize = 115.f;
	float stockSize = 25.f;
	float padding = 20.f;
	float stockPadding = 10.f;

	sf::Text text;
	text.setFont(uiFont);
	text.setString(std::to_string((int)player->getDamage()) + "%");
	text.setCharacterSize((unsigned int)textSize);
	text.setFillColor(sf::Color::White);
	text.setPosition(position);
	window.draw(text);

	for (int i = 0; i < stocks; ++i)
	{
		sf::CircleShape stock(stockSize);
		stock.setPosition(position + sf::Vector2f((stockSize * 2.f + stockPadding) * (float)i, textSize / 2.f + stockSize + padding + stockSize));
		stock.setFillColor(sf::Color::Red);
		window.draw(stock);
	}
}

void secro::Game::updateDeath(float deltaTime)
{
	if (playerManager.isGameDone() && dtScalar == 1.f)
	{
		dtScalar = deathSpeed;
		deathTimer = deathDuration;
	}

	if (deathTimer >= 0.f)
	{
		deathTimer -= deltaTime;
		if (deathTimer < 0.f)
		{
			dtScalar = 1.f;
			playerManager.resetGame();
		}
	}
}
