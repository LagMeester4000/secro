#include "Level.h"
#include "secro/framework/player/PlayerCharacter.h"
#include "secro/framework/input/InputManager.h"

using namespace secro;

secro::Level::Level(std::shared_ptr<InputManager> input)
{
	inputManager = input;
}

void secro::Level::init()
{
	for (auto& it : players)
	{
		it->init();
	}
}

void secro::Level::update(float deltaTime)
{
	updateSimulate(deltaTime);
	std::vector<PlayerCharacter*> p = { &*players[0], &*players[1] };
	camera.update(deltaTime, p);
}

void secro::Level::updateSimulate(float deltaTime)
{
	for (auto& it : players)
	{
		it->update(deltaTime);
	}
	physicsManager.update(deltaTime);
	hitboxManager.update(deltaTime, *this);
	particleSystem.update(deltaTime);
}

void secro::Level::render(sf::RenderWindow & window)
{
	//render stage
	physicsManager.debugRender(window);

	//render particles
	particleSystem.renderBack(window);

	//render players
	for (auto& it : players)
	{
		it->render(window);
	}

	//render particles
	particleSystem.renderFront(window);

	//debug rendering of hitboxes
	hitboxManager.render(window);
}

void secro::Level::cameraRender(sf::RenderWindow & window)
{
	camera.render(window);
}

void secro::Level::reset()
{
	for (auto& it : players)
	{
		it->reset({ 0.f, 0.f });
	}
}

void secro::Level::addPlayer(std::shared_ptr<PlayerCharacter> newPlayer)
{
	size_t index = players.size();
	newPlayer->lateSetup(this, &hitboxManager, physicsManager.addPlayerCollider(), inputManager->getController((int)index));
	players.push_back(newPlayer);
	newPlayer->reset({ 0.f, 0.f });
}

void secro::Level::addEntity(std::shared_ptr<Entity> newEntity)
{
}

bool secro::Level::isGameOver()
{
	return false;
}

std::shared_ptr<PlayerCharacter> secro::Level::getWinningPlayer()
{
	return std::shared_ptr<PlayerCharacter>();
}

void secro::Level::netSerSave(RawSerializeBuffer & buff)
{
}

void secro::Level::netSerLoad(RawSerializeBuffer & buff)
{
}

ParticleSystem & secro::Level::getParticleSystem()
{
	return particleSystem;
}

Camera & secro::Level::getCamera()
{
	return camera;
}

SecroPhysicsManager & secro::Level::getPhysics()
{
	return physicsManager;
}
