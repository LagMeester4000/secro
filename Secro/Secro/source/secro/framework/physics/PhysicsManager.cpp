#include "PhysicsManager.h"

secro::PhysicsManager::PhysicsManager() : world(std::make_shared<b2World>(b2Vec2{0.f, 0.f}))
{
	playersColliders.emplace_back(*world, b2Vec2(-2.f, -5.f));
	playersColliders.emplace_back(*world, b2Vec2(2.f, -5.f));
	stageColliders.emplace_back(*world, b2Vec2(0.f, 5.f), b2Vec2(8.f, 3.f));
}

secro::PhysicsManager::~PhysicsManager()
{
}

void secro::PhysicsManager::update(float deltaTime)
{
	world->Step(deltaTime, 10, 10);
}

void secro::PhysicsManager::debugRender(sf::RenderWindow& window)
{
	for (auto& it : playersColliders)
	{
		it.render(window);
	}

	for (auto& it : stageColliders)
	{
		it.render(window);
	}
}

b2Body * secro::PhysicsManager::getPlayerBody(int index)
{
	return playersColliders[index].playerBody;
}
