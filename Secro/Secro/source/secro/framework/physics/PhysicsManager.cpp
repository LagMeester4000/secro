#include "PhysicsManager.h"
#include "secro/framework/DebugOptions.h"

///TEMP
#include <imgui.h>
#include <secro/framework/math/Curve.h>
secro::Curve curve;

secro::PhysicsManager::PhysicsManager() : world(std::make_shared<b2World>(b2Vec2{0.f, 0.f}))
{
	world->SetContactListener(&listener);

	//TEMP
	curve.resultMultiplicant = -0.5f;
	curve.resultAdd = 1.f;
	curve.setFormula(Linear());
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
	if (!DebugOptions::getOptions().enablePhysicsRender)
		return;

	{
		ImGui::Begin("TEST");

		curve.renderCurveEditor();

		ImGui::End();
	}

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

void secro::PhysicsManager::addStageCollider(b2Vec2 position, b2Vec2 size, bool isPlatform)
{
	stageColliders.emplace_back(*world, position, size, isPlatform);
}

b2Body * secro::PhysicsManager::makePlayerBody()
{
	playersColliders.emplace_back(*world, b2Vec2(-2.f, -5.f));
	return playersColliders[playersColliders.size() - 1].playerBody;
}

b2Body * secro::PhysicsManager::getPlayerBody(int index)
{
	return playersColliders[index].playerBody;
}
