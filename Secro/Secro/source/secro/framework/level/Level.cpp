#include "Level.h"
#include "secro/framework/player/PlayerCharacter.h"

using namespace secro;

secro::Level::Level(std::shared_ptr<InputManager> input)
{
	inputManager = input;
}

void secro::Level::init()
{
	
}

void secro::Level::update(float deltaTime)
{
}

void secro::Level::render(sf::RenderWindow & window)
{
	physicsManager.debugRender(window);

	for (auto& it : players)
	{
		it->render(window);
	}

	hitboxManager.render(window);
	camera.render(window);
}

void secro::Level::addPlayer(std::shared_ptr<PlayerCharacter> newPlayer)
{
	players.push_back(newPlayer);
}

bool secro::Level::isGameOver()
{
	return false;
}

std::shared_ptr<PlayerCharacter> secro::Level::getWinningPlayer()
{
	return std::shared_ptr<PlayerCharacter>();
}
