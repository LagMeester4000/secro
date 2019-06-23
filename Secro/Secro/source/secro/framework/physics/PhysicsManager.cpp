#include "PhysicsManager.h"
#include "secro/framework/DebugOptions.h"
#include "secro/netplay/RawSerializeBuffer.h"
#include "secro/netplay/SerializeFunctions.h"

///TEMP
#include <imgui.h>
#include <secro/framework/math/Curve.h>
secro::Curve curve;
secro::Curve curve2;

secro::PhysicsManager::PhysicsManager() : world(std::make_shared<b2World>(b2Vec2{0.f, 0.f}))
{
	world->SetContactListener(&listener);

	//TEMP
	curve.resultMultiplicant = -0.5f;
	curve.resultAdd = 1.f;
	curve.setFormula(Linear());
	curve2.resultMultiplicant = -0.5f;
	curve2.resultAdd = 1.f;
	curve2.setFormula(Exponent());
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
		curve2.renderCurveEditor();

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

namespace secro {
	void netSerSave(secro::PlayerCollision& val, secro::RawSerializeBuffer& buff)
	{
		//b2Transform t = val.playerBody->GetTransform();
		//auto vel = val.playerBody->GetLinearVelocity();
		//auto angVel = val.playerBody->GetAngularVelocity();
		//buff.save(t);
		//buff.save(vel);
		//buff.save(angVel);

		val.playerBody->netSerSave(buff);
	}

	void netSerLoad(secro::PlayerCollision& val, secro::RawSerializeBuffer& buff)
	{
		//b2Transform t;
		//b2Vec2 vel;
		//float angVel;
		//buff.load(t);
		//buff.load(vel);
		//buff.load(angVel);
		////not sure about .GetAngle(), might cause desync
		//val.playerBody->SetTransform(t);
		//val.playerBody->SetLinearVelocityRaw(vel);
		//val.playerBody->SetAngularVelocityRaw(angVel);

		val.playerBody->netSerLoad(buff);
	}
}

void secro::PhysicsManager::netSerSave(RawSerializeBuffer & buff)
{
	secro::netSerSave(playersColliders, buff);
}

void secro::PhysicsManager::netSerLoad(RawSerializeBuffer & buff)
{
	size_t size;
	secro::netSerLoad(size, buff);

	for (size_t i = 0; i < size; ++i)
	{
		secro::netSerLoad(playersColliders[i], buff);
	}
}
