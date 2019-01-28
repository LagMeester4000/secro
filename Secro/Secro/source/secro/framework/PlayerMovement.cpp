#include "PlayerMovement.h"
#include <Box2D/Box2D.h>

secro::PlayerMovement::PlayerMovement(std::shared_ptr<Controller> controller, b2Body* playerBody)
{
}

void secro::PlayerMovement::update(float deltaTime)
{
	//landing
	if (state == MovementState::InAir)
	{
		auto& vel = physicsBody->GetLinearVelocity();
		
		//player has to be moving down to land
		if (vel.y >= 0.f)
		{
			auto* world = physicsBody->GetWorld();
			world->RayCast(&groundStickCallback, b2Vec2(), b2Vec2());
		}
	}
}

void secro::PlayerMovement::init()
{
	
}

float32 secro::PlayerMovement::RayToGround::ReportFixture(b2Fixture * fixture, const b2Vec2 & point, const b2Vec2 & normal, float32 fraction)
{
	return float32();
}
