#include "Game.h"
#include "framework/player/PlayerCharacter.h"

using namespace secro;

std::shared_ptr<Game> secro::Game::createGame()
{
	class make_shared_enabler : public Game {};

	return std::make_shared<make_shared_enabler>();
}

secro::Game::Game()
{
	inputManager.init(2);
	
	player = new PlayerCharacter(&hitboxManager, physicsManager.getPlayerBody(0), inputManager.getController(0));
	player->init();

	dummyPlayer = new PlayerCharacter(&hitboxManager, physicsManager.getPlayerBody(1), inputManager.getController(1));
	dummyPlayer->init();
}

//TEMP
#include <SFML/Window.hpp>

void secro::Game::update(float deltaTime)
{
	inputManager.update();
	player->update(deltaTime);
	dummyPlayer->update(deltaTime);
	physicsManager.update(deltaTime);
	hitboxManager.update(deltaTime);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
	{
		physicsManager.getPlayerBody(0)->SetTransform(b2Vec2(-5.f, -2.f), 0.f);
		physicsManager.getPlayerBody(1)->SetTransform(b2Vec2(5.f, -2.f), 0.f);
	}
}

void secro::Game::render(sf::RenderWindow & window)
{
	physicsManager.debugRender(window);
	player->render(window);
	dummyPlayer->render(window);

	//TEMP
	c.renderFrameDataEditor(window);
	hitboxManager.render(window);
}
