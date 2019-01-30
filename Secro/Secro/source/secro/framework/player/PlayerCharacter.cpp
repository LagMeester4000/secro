#include "PlayerCharacter.h"
#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include "../input/Controller.h"
#include "../collision/HitboxCollection.h"
#include "../collision/HitboxManager.h"
#include "../physics/Filters.h"
#include "../detail/PlainVectorMath.h"

using namespace secro;

secro::PlayerCharacter::PlayerCharacter()
{
}

secro::PlayerCharacter::PlayerCharacter(HitboxManager* hitboxM, b2Body * body, std::shared_ptr<Controller> controller)
{
	hitboxManager = hitboxM;
	physicsBody = body;
	input = controller;
	facingDirection = FacingDirection::Right;
}

void secro::PlayerCharacter::init()
{
	//setup testing attributes
	auto& a = attributes;
	a.airAcceleration = 2000.f;
	a.airDeceleration = 1.f;
	a.airMaxSpeed = 10.f;
	a.dashDuration = 0.3f;
	a.dashInitialSpeed = 10.f;
	a.doubleJumpSpeed = 20.f;
	a.fallSpeed = 3000.f;
	a.fastfallSpeed = 10.f;
	a.groundDeceleration = 3000.f;
	a.jumpAmount = 1;
	a.jumpFullSpeed = 17.f;
	a.jumpShortSpeed = 10.f;
	a.jumpSquatDuration = 0.05f;
	a.runAcceleration = 1000.f;
	a.runMaxSpeed = 8.f;
	a.walkMaxSpeed = 3.f;

	state = PlayerState::Jump;
	movementState = MovementState::InAir;

	//call state machine setup
	setupStates(stateMachine);

	//setup the attacks
	setupAttacks(attackCollection);


	//temp
	auto hurt = attackCollection.loadRaw("Hurt.json");
	hitboxManager->addHurtbox(this, hurt);
}

void secro::PlayerCharacter::setupStates(StateMachine & sm)
{
	//jumping
	sm.addCondition(PlayerState::Jump, PlayerState::Stand, [&](float t)
	{
		if (getMovementState() == MovementState::OnGround)
			return true;
		return false;
	});

	//condition for starting to jump from standing, running, or dashing
	auto startJumpSquat = [&](float t)
	{
		return getInput()->jumpPressed();
	};
	sm.addCondition(PlayerState::Stand, PlayerState::JumpSquat, startJumpSquat);
	sm.addCondition(PlayerState::Run, PlayerState::JumpSquat, startJumpSquat);
	sm.addCondition(PlayerState::Dash, PlayerState::JumpSquat, startJumpSquat);
	sm.addCondition(PlayerState::Walk, PlayerState::JumpSquat, startJumpSquat);

	//condition for entering jump
	sm.addCondition(PlayerState::JumpSquat, PlayerState::Jump, [&](float t)
	{
		return IsStateTimerDone();
	});

	//on jump squat activation
	sm.addSetState(PlayerState::JumpSquat, std::bind(&PlayerCharacter::stateStartJumpSquat, this));
	//on jump activation
	//sm.addSetState(PlayerState::Jump, std::bind(&PlayerCharacter::stateStartJump, this));
	sm.addUnsetState(PlayerState::JumpSquat, std::bind(&PlayerCharacter::stateStartJump, this));


	//dash
	sm.addSetState(PlayerState::Dash, std::bind(&PlayerCharacter::stateStartDash, this));
	sm.addCondition(PlayerState::Dash, PlayerState::Dash, [&](float f)
	{
		auto input = getInput();
		auto mov = input->getMovementDirection();
		auto smash = input->getMovementPushDirectionExt();
		auto face = getFacingDirection();
		return smash.speed > 30.f && ((mov == Direction::Left && face == FacingDirection::Right) || (mov == Direction::Right && face == FacingDirection::Left));
	});
	sm.addCondition(PlayerState::Dash, PlayerState::Run, [&](float f)
	{
		auto input = getInput();
		auto mov = input->getMovementDirection();
		auto face = getFacingDirection();
		return IsStateTimerDone() &&
			((mov == Direction::Right && face == FacingDirection::Right) || (mov == Direction::Left && face == FacingDirection::Left));
	});
	sm.addCondition(PlayerState::Stand, PlayerState::Dash, [&](float f)
	{
		auto input = getInput();
		auto smash = input->getMovementPushDirectionExt();
		auto mov = input->getMovementDirection();
		return smash.speed > 30.f && ((mov == Direction::Left) || (mov == Direction::Right));
	});
	sm.addCondition(PlayerState::Dash, PlayerState::Stand, [&](float f)
	{
		return IsStateTimerDone();// && getPhysicsBody()->GetLinearVelocity().Length() < 0.1f;
	});


	//run
	sm.addCondition(PlayerState::Run, PlayerState::Stand, [&](float f)
	{
		return getPhysicsBody()->GetLinearVelocity().Length() < 0.1f;
	});


	//walk
	sm.addCondition(PlayerState::Stand, PlayerState::Walk, [&](float f)
	{
		auto input = getInput();
		auto smash = input->getMovementPushDirectionExt();
		auto mov = input->getMovementDirection();
		return smash.speed <= 30.f && ((mov == Direction::Left) || (mov == Direction::Right));
	});
	sm.addCondition(PlayerState::Walk, PlayerState::Stand, [&](float f)
	{
		auto input = getInput();
		auto mov = input->getMovementDirection();
		return !isEqual(getFacingDirection(), mov);
	});
	sm.addSetState(PlayerState::Walk, std::bind(&PlayerCharacter::stateStartWalk, this));


	//attacks
	auto endAerialA = [&](float f)
	{
		return getMovementState() == MovementState::OnGround || IsStateTimerDone();
	};
	//nair
	sm.addCondition(PlayerState::Jump, PlayerState::ANAir, [&](float f)
	{
		auto input = getInput();
		auto dir = input->getMovementDirection();
		return dir == Direction::Neutral && input->attackPressed();
	});
	sm.addSetState(PlayerState::ANAir, std::bind(&PlayerCharacter::stateStartNewAttack, this, PlayerState::ANAir));
	sm.addUnsetState(PlayerState::ANAir, std::bind(&PlayerCharacter::endAttack, this));
	sm.addCondition(PlayerState::ANAir, PlayerState::LandingLag, endAerialA);
	//uair
	sm.addCondition(PlayerState::Jump, PlayerState::AUAir, [&](float f)
	{
		auto input = getInput();
		auto dir = input->getMovementDirection();
		auto atDir = input->getDirAttackDirection();
		return (dir == Direction::Up && input->attackPressed()) || atDir == Direction::Up;
	});
	sm.addSetState(PlayerState::AUAir, std::bind(&PlayerCharacter::stateStartNewAttack, this, PlayerState::AUAir));
	sm.addUnsetState(PlayerState::AUAir, std::bind(&PlayerCharacter::endAttack, this));
	sm.addCondition(PlayerState::AUAir, PlayerState::LandingLag, endAerialA);
	//fair
	sm.addCondition(PlayerState::Jump, PlayerState::AFAir, [&](float f)
	{
		auto input = getInput();
		auto dir = input->getMovementDirection();
		auto atDir = input->getDirAttackDirection();
		return isEqual(getFacingDirection(), dir) && input->attackPressed() || isEqual(getFacingDirection(), atDir);
	});
	sm.addSetState(PlayerState::AFAir, std::bind(&PlayerCharacter::stateStartNewAttack, this, PlayerState::AFAir));
	sm.addUnsetState(PlayerState::AFAir, std::bind(&PlayerCharacter::endAttack, this));
	sm.addCondition(PlayerState::AFAir, PlayerState::LandingLag, endAerialA);
	//bair
	sm.addCondition(PlayerState::Jump, PlayerState::ABAir, [&](float f)
	{
		auto input = getInput();
		auto dir = input->getMovementDirection();
		auto atDir = input->getDirAttackDirection();
		return isOpposite(getFacingDirection(), dir) && input->attackPressed() || isOpposite(getFacingDirection(), atDir);
	});
	sm.addSetState(PlayerState::ABAir, std::bind(&PlayerCharacter::stateStartNewAttack, this, PlayerState::ABAir));
	sm.addUnsetState(PlayerState::ABAir, std::bind(&PlayerCharacter::endAttack, this));
	sm.addCondition(PlayerState::ABAir, PlayerState::LandingLag, endAerialA);
	//dair
	sm.addCondition(PlayerState::Jump, PlayerState::ADAir, [&](float f)
	{
		auto input = getInput();
		auto dir = input->getMovementDirection();
		auto atDir = input->getDirAttackDirection();
		return dir == Direction::Down && input->attackPressed() || atDir == Direction::Down;
	});
	sm.addSetState(PlayerState::ADAir, std::bind(&PlayerCharacter::stateStartNewAttack, this, PlayerState::ADAir));
	sm.addUnsetState(PlayerState::ADAir, std::bind(&PlayerCharacter::endAttack, this));
	sm.addCondition(PlayerState::ADAir, PlayerState::LandingLag, endAerialA);

	//ground attacks
	auto endGroundAttack = [&](float f) {
		return IsStateTimerDone();
	};
	//utilt
	auto UTiltAttack = [&](float f)
	{
		auto input = getInput();
		auto dir = input->getMovementDirection();
		return dir == Direction::Up && input->attackPressed();
	};
	sm.addCondition(PlayerState::Stand, PlayerState::AUTilt, UTiltAttack);
	sm.addCondition(PlayerState::Walk, PlayerState::AUTilt, UTiltAttack);
	sm.addCondition(PlayerState::Run, PlayerState::AUTilt, UTiltAttack);
	sm.addSetState(PlayerState::AUTilt, std::bind(&PlayerCharacter::stateStartNewAttack, this, PlayerState::AUTilt));
	sm.addUnsetState(PlayerState::AUTilt, std::bind(&PlayerCharacter::endAttack, this));
	sm.addCondition(PlayerState::AUTilt, PlayerState::Stand, endGroundAttack);
	//ftilt
	auto FTiltAttack = [&](float f)
	{
		auto input = getInput();
		auto dir = input->getMovementDirection();
		return isEqual(getFacingDirection(), dir) && input->attackPressed();
	};
	sm.addCondition(PlayerState::Stand, PlayerState::AFTilt, FTiltAttack);
	sm.addCondition(PlayerState::Walk, PlayerState::AFTilt, FTiltAttack);
	sm.addCondition(PlayerState::Run, PlayerState::AFTilt, FTiltAttack);
	sm.addSetState(PlayerState::AFTilt, std::bind(&PlayerCharacter::stateStartNewAttack, this, PlayerState::AFTilt));
	sm.addUnsetState(PlayerState::AFTilt, std::bind(&PlayerCharacter::endAttack, this));
	sm.addCondition(PlayerState::AFTilt, PlayerState::Stand, endGroundAttack);
	//dtilt
	auto DTiltAttack = [&](float f)
	{
		auto input = getInput();
		auto dir = input->getMovementDirection();
		return dir == Direction::Down && input->attackPressed();
	};
	sm.addCondition(PlayerState::Stand, PlayerState::ADTilt, DTiltAttack);
	sm.addCondition(PlayerState::Walk, PlayerState::ADTilt, DTiltAttack);
	sm.addCondition(PlayerState::Run, PlayerState::ADTilt, DTiltAttack);
	sm.addSetState(PlayerState::ADTilt, std::bind(&PlayerCharacter::stateStartNewAttack, this, PlayerState::ADTilt));
	sm.addUnsetState(PlayerState::ADTilt, std::bind(&PlayerCharacter::endAttack, this));
	sm.addCondition(PlayerState::ADTilt, PlayerState::Stand, endGroundAttack);


	//landing lag
	sm.addCondition(PlayerState::LandingLag, PlayerState::Stand, [&](float f) 
	{
		return IsStateTimerDone();
	});
	sm.addCondition(PlayerState::LandingLag, PlayerState::Jump, [&](float f)
	{
		return getMovementState() == MovementState::InAir;
	});

	//hitstun
	sm.addCondition(PlayerState::Hitstun, PlayerState::Jump, [&](float f) 
	{
		return !IsInHitstun();
	});
}

void secro::PlayerCharacter::setupAttacks(AttackCollection & atts)
{
	atts.loadAttack("NAir.json", PlayerState::ANAir);
	atts.loadAttack("UAir.json", PlayerState::AUAir);
	atts.loadAttack("FAir.json", PlayerState::AFAir);
	atts.loadAttack("BAir.json", PlayerState::ABAir);
	atts.loadAttack("DAir.json", PlayerState::ADAir);

	atts.loadAttack("UTilt.json", PlayerState::AUTilt);
	atts.loadAttack("FTilt.json", PlayerState::AFTilt);
	atts.loadAttack("DTilt.json", PlayerState::ADTilt);
}

void secro::PlayerCharacter::update(float deltaTime)
{
	updateHitlag(deltaTime);

	if (!isInHitlag())
	{
		updateMovement(deltaTime);
		updateHitstun(deltaTime);
		updateState(deltaTime);
		updateAttack(deltaTime);
	}
	
	//temp kill plane
	auto pos = physicsBody->GetPosition();
	if (pos.x < -15.f || pos.x > 10.f || pos.y < -10.f || pos.y > 10.f)
	{
		auto newPos = b2Vec2{ 0.f ,0.f };
		physicsBody->SetTransform(newPos, 0.f);
		damage = debugDamage;
	}

	if (input->grabPressed())
	{
		physicsBody->SetTransform(b2Vec2(0.f,0.f), 0.f);
		setupAttacks(attackCollection);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
	{
		damage = 0.f;
	}
}

void secro::PlayerCharacter::render(sf::RenderWindow & window)
{
	//draw attributes gui
	debugRenderAttributes(window);

	sf::CircleShape c(0.4f);
	auto pos = getPosition();
	c.setPosition({ pos.x - 0.4f, pos.y - 0.4f });

	//decide collor
	c.setFillColor(sf::Color(255, 255, 255));
	if (state == PlayerState::Dash)
	{
		c.setFillColor(sf::Color(255, 100, 0));
	}
	else if (state == PlayerState::Run)
	{
		c.setFillColor(sf::Color(0, 255, 0));
	}
	else if (state == PlayerState::Stand)
	{
		c.setFillColor(sf::Color(0, 0, 255));
	}
	else if (state == PlayerState::Jump)
	{
		c.setFillColor(sf::Color(0, 0, 0));
	}
	else if (state == PlayerState::JumpSquat)
	{
		c.setFillColor(sf::Color(100, 100, 100));
	}

	window.draw(c);


	//facing thingy
	sf::CircleShape c2(0.2f);
	c2.setFillColor(sf::Color(255, 255, 255));

	if (facingDirection == FacingDirection::Right)
	{
		c2.setPosition({ pos.x - 0.2f + 1.f, pos.y - 0.2f });
	}
	else
	{
		c2.setPosition({ pos.x - 0.2f - 1.f, pos.y - 0.2f });
	}

	window.draw(c2);
}

b2Vec2 secro::PlayerCharacter::getPosition()
{
	return physicsBody->GetPosition();
}

void secro::PlayerCharacter::updateMovement(float deltaTime)
{
	switch (movementState)
	{
	case MovementState::InAir:
	{
		//double jumps
		if (state == PlayerState::Jump)
		{
			if (input->jumpPressed() && jumpsLeft > 0)
			{
				jumpsLeft--;
				b2Vec2 jumpVel = { 0.f, -attributes.doubleJumpSpeed };
				physicsBody->SetLinearVelocity(jumpVel);
			}
		}

		//fastfall
		if (input->getMovementPushDirection() == Direction::Down)
		{
			auto vel = physicsBody->GetLinearVelocity();
			if (vel.y > 0.f)
			{
				physicsBody->SetLinearVelocity(b2Vec2(vel.x, attributes.fastfallSpeed));
			}
		}

		//update gravity
		{
			auto cvel = physicsBody->GetLinearVelocity();
			physicsBody->SetLinearVelocity(b2Vec2(cvel.x, cvel.y + attributes.fallSpeed * deltaTime * deltaTime));
		}

		auto mdir = input->getMovementDirection();
		if (mdir != Direction::Neutral)
		{
			if (mdir == Direction::Left)
			{
				//maybe setting velocity wrong !!!
				auto vel = physicsBody->GetLinearVelocity();

				b2Vec2 unitVec = { -attributes.airAcceleration * deltaTime * deltaTime, 0.f };

				if (vel.x > -attributes.airMaxSpeed)
				{
					vel += unitVec;
					physicsBody->SetLinearVelocity(vel);
				}
			}
			else if (mdir == Direction::Right)
			{
				//maybe setting velocity wrong !!!
				auto vel = physicsBody->GetLinearVelocity();

				b2Vec2 unitVec = { attributes.airAcceleration * deltaTime * deltaTime, 0.f };

				if (vel.x < attributes.airMaxSpeed)
				{
					vel += unitVec;
					physicsBody->SetLinearVelocity(vel);
				}
			}

		}
		

		//apply tracktion when needed
		if (input->getMovementDirection() == Direction::Neutral)
		{
			auto decelVal = attributes.airDeceleration * deltaTime * deltaTime;
			auto vel = physicsBody->GetLinearVelocity();

			if (vel.Length() > decelVal)
			{
				auto adder = vel;
				adder.Normalize();
				adder *= decelVal;
				vel -= adder;
			}
			else
			{
				vel = { 0.f, 0.f };
			}

			physicsBody->SetLinearVelocity(vel);
		}
		

		//shap to ground
		if (snapToGround(0.3f))
		{
			movementState = MovementState::OnGround;
		}

	} break;
	case MovementState::OnLedge:
	{

	} break;
	case MovementState::OnGround:
	{
		//reset jump counter
		jumpsLeft = attributes.jumpAmount;

		//run/walk acceleration
		if (state == PlayerState::Run || state == PlayerState::Walk)
		{
			float maxSpeed;
			if (state == PlayerState::Run)
				maxSpeed = attributes.runMaxSpeed;
			else
				maxSpeed = attributes.walkMaxSpeed;

			if (keepRunning())
			{
				if (facingDirection == FacingDirection::Left)
				{
					//maybe setting velocity wrong !!!
					auto vel = physicsBody->GetLinearVelocity();
					
					b2Vec2 unitVec = { -attributes.runAcceleration * deltaTime * deltaTime, 0.f };

					if (abs(vel.x) < maxSpeed)
					{
						vel += unitVec;
						physicsBody->SetLinearVelocity(vel);
					}
				}
				else
				{
					//maybe setting velocity wrong !!!
					auto vel = physicsBody->GetLinearVelocity();

					b2Vec2 unitVec = { attributes.runAcceleration * deltaTime * deltaTime, 0.f };

					if (abs(vel.x) < maxSpeed)
					{
						vel += unitVec;
						physicsBody->SetLinearVelocity(vel);
					}
				}

			}
		}

		//apply tracktion when needed
		if ((!keepRunning() || groundSpeedTooHigh()) && state != PlayerState::Dash)
		{
			auto decelVal = attributes.groundDeceleration * deltaTime * deltaTime;
			auto vel = physicsBody->GetLinearVelocity();

			if (vel.Length() > decelVal)
			{
				auto adder = vel;
				adder.Normalize();
				adder *= decelVal;
				vel -= adder;
			}
			else
			{
				vel = { 0.f, 0.f };
			}

			physicsBody->SetLinearVelocity(vel);
		}

		//shap to ground
		if (!snapToGround(0.3f))
		{
			movementState = MovementState::InAir;
			stateMachine.unsafeChangeState(this, PlayerState::Jump, deltaTime);
		}
	} break;
	}
}

bool secro::PlayerCharacter::snapToGround(float distance)
{
	auto vel = physicsBody->GetLinearVelocity();
	
	//don't want to snap to the ground if we're moving upwards
	if (vel.y < 0.f && movementState != MovementState::OnGround)
		return false;

	// this class captures the closest hit shape.
	class IgnorePlayerRay : public b2RayCastCallback
	{
	public:
		IgnorePlayerRay()
		{
			m_fixture = NULL;
		}

		float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point,
			const b2Vec2& normal, float32 fraction)
		{
			if (fixture)
			{
				if (fixture->GetFilterData().groupIndex == GROUP_PLAYER)
					return -1;
			}

			m_fixture = fixture;
			m_point = point;
			m_normal = normal;
			m_fraction = fraction;
			return fraction;
		}

		b2Fixture* m_fixture;
		b2Vec2 m_point;
		b2Vec2 m_normal;
		float32 m_fraction;
		bool firstHit = true;
	};
	auto* world = physicsBody->GetWorld();

	auto callBack = IgnorePlayerRay();
	b2Vec2 p1 = getPosition() + b2Vec2{ 0.f, 0.8f };
	b2Vec2 p2 = p1 + b2Vec2{ 0.f, distance + 0.2f };

	world->RayCast(&callBack, p1, p2);

	if (callBack.m_fixture != nullptr)
	{
		b2Vec2 oldPos = physicsBody->GetPosition();
		b2Vec2 newPos = b2Vec2{ oldPos.x, callBack.m_point.y - 1.f };
		physicsBody->SetTransform(newPos, 0.f);
		return true;
	}
	return false;
}

bool secro::PlayerCharacter::keepRunning()
{
	return isEqual(facingDirection, input->getMovementDirection());
}

void secro::PlayerCharacter::stateStartJump()
{
	movementState = MovementState::InAir;

	bool fullHop = input->jumpHeld();

	auto currentVel = physicsBody->GetLinearVelocity();
	if (fullHop)
		physicsBody->SetLinearVelocity({ currentVel.x, -attributes.jumpFullSpeed });
	else
		physicsBody->SetLinearVelocity({ currentVel.x, -attributes.jumpShortSpeed });
}

void secro::PlayerCharacter::stateStartJumpSquat()
{
	stateTimer = attributes.jumpSquatDuration;
}

void secro::PlayerCharacter::stateStartDash()
{
	auto dir = input->getMovementDirection();
	
	if (dir == Direction::Left)
	{
		facingDirection = FacingDirection::Left;
		physicsBody->SetLinearVelocity(b2Vec2(-attributes.dashInitialSpeed, 0.f));
	}
	else if (dir == Direction::Right)
	{
		facingDirection = FacingDirection::Right;
		physicsBody->SetLinearVelocity(b2Vec2(attributes.dashInitialSpeed, 0.f));
	}

	stateTimer = attributes.dashDuration;
}

void secro::PlayerCharacter::stateStartWalk()
{
	auto dir = input->getMovementDirection();

	if (dir == Direction::Left)
	{
		facingDirection = FacingDirection::Left;
	}
	else if (dir == Direction::Right)
	{
		facingDirection = FacingDirection::Right;
	}
}

void secro::PlayerCharacter::endAttack()
{
	currentAttackHitbox->destroy();
	currentAttackHitbox = nullptr;

	if (movementState == MovementState::OnGround)
	{
		auto& at = attackCollection.getAttack(state);
		stateTimer = at.landingLag;
	}
}

bool secro::PlayerCharacter::groundSpeedTooHigh()
{
	auto vel = physicsBody->GetLinearVelocity();
	
	if (state == PlayerState::Run)
		return vel.Length() > attributes.runMaxSpeed;
	else if (state == PlayerState::Walk)
		return vel.Length() > attributes.walkMaxSpeed;

	return false;
}

void secro::PlayerCharacter::debugRenderAttributes(sf::RenderWindow & window)
{
	ImGui::PushID(this);
	std::string name = "Player Movement Attributes" + std::to_string((long long int)this);
	if (ImGui::Begin(name.c_str()))
	{
		//TEMP
		ImGui::Text("Damage: %f", damage);
		ImGui::InputFloat("Respawn Damage", &debugDamage);
		ImGui::Separator();

		ImGui::Text("Air");
		ImGui::InputFloat("FallSpeed", &attributes.fallSpeed);
		ImGui::InputFloat("AirAcceleration", &attributes.airAcceleration);
		ImGui::InputFloat("AirDeceleration", &attributes.airDeceleration);
		ImGui::InputFloat("AirMaxSpeed", &attributes.airMaxSpeed);
		ImGui::InputFloat("FastFallSpeed", &attributes.fastfallSpeed);

		ImGui::Separator();

		ImGui::Text("Jump");
		ImGui::InputInt("JumpAmount", &attributes.jumpAmount);
		ImGui::InputFloat("JumpFullSpeed", &attributes.jumpFullSpeed);
		ImGui::InputFloat("JumpShortSpeed", &attributes.jumpShortSpeed);
		ImGui::InputFloat("DoubleJumpSpeed", &attributes.doubleJumpSpeed);
		ImGui::InputFloat("JumpSquatDuration", &attributes.jumpSquatDuration);

		ImGui::Separator();

		ImGui::Text("Dash");
		ImGui::InputFloat("DashDuration", &attributes.dashDuration);
		ImGui::InputFloat("DashInitialSpeed", &attributes.dashInitialSpeed);

		ImGui::Separator();

		ImGui::Text("Ground Movement");
		ImGui::InputFloat("RunAcceleration", &attributes.runAcceleration);
		ImGui::InputFloat("WalkMaxSpeed", &attributes.walkMaxSpeed);
		ImGui::InputFloat("RunMaxSpeed", &attributes.runMaxSpeed);
		ImGui::InputFloat("GroundDeceleration", &attributes.groundDeceleration);

		ImGui::End();
	}
	ImGui::PopID();
}

MovementState secro::PlayerCharacter::getMovementState()
{
	return movementState;
}

const PlayerAttributes & secro::PlayerCharacter::getPlayerAttributes()
{
	return attributes;
}

FacingDirection secro::PlayerCharacter::getFacingDirection()
{
	return facingDirection;
}

const b2Body * secro::PlayerCharacter::getPhysicsBody()
{
	return physicsBody;
}

bool secro::PlayerCharacter::isEqual(FacingDirection facing, Direction dir)
{
	return (facing == FacingDirection::Right && dir == Direction::Right) || (facing == FacingDirection::Left && dir == Direction::Left);
}

bool secro::PlayerCharacter::isOpposite(FacingDirection facing, Direction dir)
{
	return  (facing == FacingDirection::Right && dir == Direction::Left) || (facing == FacingDirection::Left && dir == Direction::Right);
}

void secro::PlayerCharacter::knockBack(b2Vec2 knockback)
{
	if (knockback.y > 0.f && movementState == MovementState::OnGround)
	{
		knockback.y = -knockback.y;
		knockback = mul(knockback, 0.8f);
	}

	if (knockback.y < 0)
		movementState = MovementState::InAir;

	physicsBody->SetLinearVelocity(knockback);
}

void secro::PlayerCharacter::updateHitstun(float deltaTime)
{
	if (hitstun >= 0.f)
		hitstun -= deltaTime;
}

bool secro::PlayerCharacter::IsInHitstun()
{
	return hitstun > 0.f;
}

void secro::PlayerCharacter::putInHitstun(float duration)
{
	stateMachine.changeState(this, PlayerState::Hitstun, 0.016f);
	stateTimer = duration;
	hitstun = duration;
}

void secro::PlayerCharacter::updateState(float deltaTime)
{
	if (stateTimer >= 0.f)
		stateTimer -= deltaTime;

	stateMachine.update(deltaTime, this);
}

PlayerState secro::PlayerCharacter::getState()
{
	return state;
}

bool secro::PlayerCharacter::IsStateTimerDone()
{
	return stateTimer <= 0.f;
}

void secro::PlayerCharacter::setStateTimer(float seconds)
{
	stateTimer = seconds;
}

std::shared_ptr<Controller> secro::PlayerCharacter::getInput()
{
	return input;
}

float secro::PlayerCharacter::getDamage()
{
	return 0.0f;
}

float secro::PlayerCharacter::getDamageScalar()
{
	return damage / 100.f;
}

void secro::PlayerCharacter::addDamage(float damage)
{
	this->damage += damage;
}

void secro::PlayerCharacter::updateAttack(float deltaTime)
{
	//if attacking
	if ((int)state > (int)PlayerState::AFirstAttack && (int)state < (int)PlayerState::ALastAttack)
	{
		if (currentAttackHitbox != nullptr)
		{
			auto& currentMove = attackCollection.getAttack(state);
			auto& frames = currentMove.frames;
			float nextAttackTimer = attackTimer + deltaTime;

			//push new frames to hitbox if needed
			for (auto& it : frames)
			{
				if (it.time > attackTimer && it.time <= nextAttackTimer)
				{
					currentAttackHitbox->changeHitboxes(it.changes);
				}
			}

			//update the timer
			attackTimer = nextAttackTimer;
		}
		else
		{
			std::cout << "ERROR, player is in attacking state, but does not have a current hitbox" << std::endl;
		}
	}
}

float secro::PlayerCharacter::getAttackTimer()
{
	return attackTimer;
}

int & secro::PlayerCharacter::getLastHitId()
{
	return lastHitId;
}

void secro::PlayerCharacter::updateHitlag(float deltaTime)
{
	if (hitlag > 0.f)
	{
		hitlag -= deltaTime;

		if (hitlag <= 0.f)
		{
			physicsBody->SetLinearVelocity(hitlagVelocity);
		}
	}
}

bool secro::PlayerCharacter::isInHitlag()
{
	return hitlag > 0.f;
}

void secro::PlayerCharacter::putInHitlag(float duration)
{
	hitlag = duration;
	hitlagVelocity = physicsBody->GetLinearVelocity();
	physicsBody->SetLinearVelocity(b2Vec2(0.f, 0.f));
}

void secro::PlayerCharacter::stateStartNewAttack(PlayerState attack)
{
	if (currentAttackHitbox != nullptr)
	{
		currentAttackHitbox->destroy();
		currentAttackHitbox = nullptr;
	}

	auto& att = attackCollection.getAttack(attack);
	currentAttackHitbox = hitboxManager->addHitbox(this, att);
	attackTimer = -0.01f;
	stateTimer = att.duration;
}
