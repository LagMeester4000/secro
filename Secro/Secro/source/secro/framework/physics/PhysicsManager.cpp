#include "PhysicsManager.h"

secro::PhysicsManager::PhysicsManager() : world(std::make_shared<b2World>(b2Vec2{0.f, 0.f}))
{
	playersColliders.emplace_back(*world, b2Vec2(-2.f, -5.f));
	playersColliders.emplace_back(*world, b2Vec2(2.f, -5.f));

	//main ground
	stageColliders.emplace_back(*world, b2Vec2(0.f, 5.f), b2Vec2(8.f, 3.f));

	//platforms
	stageColliders.emplace_back(*world, b2Vec2(0.f, -3.3f), b2Vec2(2.f, 0.3f), true);
	stageColliders.emplace_back(*world, b2Vec2(4.f, -0.6f), b2Vec2(2.f, 0.3f), true);
	stageColliders.emplace_back(*world, b2Vec2(-4.f, -0.6f), b2Vec2(2.f, 0.3f), true);
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
	int i = 0;
	for (auto& it : playersColliders)
	{
		it.render(window, i);
		i++;
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
