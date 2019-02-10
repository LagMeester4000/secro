#include "CharacterDashette.h"
#include "secro/framework/detail/PlainVectorMath.h"
#include <Box2D/Box2D.h>
#include <functional>

#include <tuple>

secro::CharacterDashette::CharacterDashette(HitboxManager * hitboxManager, b2Body * body, std::shared_ptr<Controller> controller)
	: PlayerCharacter(hitboxManager, body, controller)
{
}

void secro::CharacterDashette::init()
{
	PlayerCharacter::init();
	
	specialDuration = 0.20f;
	specialSpeed = 15.f;
	specialRemainSpeed = 0.4f;
	specialGroundFriction = -100.f;
	specialHyperJumpPower = 3.f;
	specialHyperJumpHeight = 14.f;
	specialAmountOfAirDash = 3;

	normalFriction = attributes.groundDeceleration;
}

void secro::CharacterDashette::setupStates(StateMachine & sm)
{
	PlayerCharacter::setupStates(sm);

	sm.addCondition(PlayerState::Jump, PlayerState::SpecialN, std::bind(&CharacterDashette::canBeginSpecial, this, std::placeholders::_1));
	sm.addCondition(PlayerState::Stand, PlayerState::SpecialN, std::bind(&CharacterDashette::canBeginSpecial, this, std::placeholders::_1));
	sm.addCondition(PlayerState::Walk, PlayerState::SpecialN, std::bind(&CharacterDashette::canBeginSpecial, this, std::placeholders::_1));
	sm.addCondition(PlayerState::Dash, PlayerState::SpecialN, std::bind(&CharacterDashette::canBeginSpecial, this, std::placeholders::_1));
	sm.addCondition(PlayerState::Run, PlayerState::SpecialN, std::bind(&CharacterDashette::canBeginSpecial, this, std::placeholders::_1));

	//attack into special cancel
	sm.addCondition(PlayerState::AUTilt, PlayerState::SpecialN, std::bind(&CharacterDashette::canBeginSpecialFromAttack, this, std::placeholders::_1));
	sm.addCondition(PlayerState::AFTilt, PlayerState::SpecialN, std::bind(&CharacterDashette::canBeginSpecialFromAttack, this, std::placeholders::_1));
	sm.addCondition(PlayerState::ADTilt, PlayerState::SpecialN, std::bind(&CharacterDashette::canBeginSpecialFromAttack, this, std::placeholders::_1));
	sm.addCondition(PlayerState::AUAir, PlayerState::SpecialN, std::bind(&CharacterDashette::canBeginSpecialFromAttack, this, std::placeholders::_1));
	sm.addCondition(PlayerState::AFAir, PlayerState::SpecialN, std::bind(&CharacterDashette::canBeginSpecialFromAttack, this, std::placeholders::_1));
	sm.addCondition(PlayerState::ABAir, PlayerState::SpecialN, std::bind(&CharacterDashette::canBeginSpecialFromAttack, this, std::placeholders::_1));
	sm.addCondition(PlayerState::ADAir, PlayerState::SpecialN, std::bind(&CharacterDashette::canBeginSpecialFromAttack, this, std::placeholders::_1));
	sm.addCondition(PlayerState::ANAir, PlayerState::SpecialN, std::bind(&CharacterDashette::canBeginSpecialFromAttack, this, std::placeholders::_1));
	sm.addCondition(PlayerState::AGrab, PlayerState::SpecialN, std::bind(&CharacterDashette::canBeginSpecialFromAttack, this, std::placeholders::_1));
	sm.addCondition(PlayerState::AGrabAir, PlayerState::SpecialN, std::bind(&CharacterDashette::canBeginSpecialFromAttack, this, std::placeholders::_1));

	//instant attack
	sm.addCondition(PlayerState::SpecialN, PlayerState::ANAir, [&](float f) 
	{
		auto dir = input->getMovementDirection();
		return dir == Direction::Neutral && input->attackPressed();
	});
	sm.addCondition(PlayerState::SpecialN, PlayerState::AUAir, [&](float f)
	{
		auto dir = input->getMovementDirection();
		auto adir = input->getDirAttackDirection();
		return dir == Direction::Up && input->attackPressed() || adir == Direction::Up;
	});
	sm.addCondition(PlayerState::SpecialN, PlayerState::AFAir, [&](float f)
	{
		auto dir = input->getMovementDirection();
		auto adir = input->getDirAttackDirection();
		return isEqual(getFacingDirection(), dir) && input->attackPressed() || isEqual(getFacingDirection(), adir);
	});
	sm.addCondition(PlayerState::SpecialN, PlayerState::ABAir, [&](float f)
	{
		auto dir = input->getMovementDirection();
		auto adir = input->getDirAttackDirection();
		return isOpposite(getFacingDirection(), dir) && input->attackPressed() || isOpposite(getFacingDirection(), adir);
	});
	sm.addCondition(PlayerState::SpecialN, PlayerState::ADAir, [&](float f)
	{
		auto dir = input->getMovementDirection();
		auto adir = input->getDirAttackDirection();
		return dir == Direction::Down && input->attackPressed() || adir == Direction::Down;
	});

	//hyperjump
	sm.addCondition(PlayerState::SpecialN, PlayerState::SpecialU, [&](float f) 
	{
		return getMovementState() == MovementState::OnGround && input->jumpPressed();
	});
	sm.addUnsetState(PlayerState::SpecialU, std::bind(&CharacterDashette::stateStartHyperJump, this));
	sm.addCondition(PlayerState::SpecialU, PlayerState::Jump, [&](float f) 
	{
		return true;
	});


	sm.addSetState(PlayerState::SpecialN, std::bind(&CharacterDashette::stateStartSpecial, this));
	sm.addUnsetState(PlayerState::SpecialN, std::bind(&CharacterDashette::stateEndSpecial, this));
	sm.addCondition(PlayerState::SpecialN, PlayerState::Jump, [&](float f) 
	{
		return IsStateTimerDone();
	});
}

void secro::CharacterDashette::setupAttacks(AttackCollection & atts)
{
	//PlayerCharacter::setupAttacks(attacks);

	atts.loadAttack("D_NAir.json", PlayerState::ANAir);
	atts.loadAttack("D_UAir.json", PlayerState::AUAir);
	atts.loadAttack("D_FAir.json", PlayerState::AFAir);
	atts.loadAttack("BAir.json", PlayerState::ABAir);
	atts.loadAttack("D_DAir.json", PlayerState::ADAir);

	atts.loadAttack("D_UTilt.json", PlayerState::AUTilt);
	atts.loadAttack("FTilt.json", PlayerState::AFTilt);
	atts.loadAttack("DTilt.json", PlayerState::ADTilt);

	atts.loadAttack("Grab.json", PlayerState::AGrab);
	atts.loadAttack("Grab.json", PlayerState::AGrabAir);
}

void secro::CharacterDashette::update(float deltaTime)
{
	PlayerCharacter::update(deltaTime);

	if (movementState == MovementState::OnGround)
	{
		airDashLeft = specialAmountOfAirDash;
	}
}

void secro::CharacterDashette::render(sf::RenderWindow & window)
{
	PlayerCharacter::render(window);
}

int secro::CharacterDashette::getAirDashLeft()
{
	return airDashLeft;
}

bool secro::CharacterDashette::canBeginSpecial(float dt)
{
	return input->specialPressed() && getAirDashLeft() > 0;
}

bool secro::CharacterDashette::canBeginSpecialFromAttack(float dt)
{
	return getHasAttackHit() && input->specialHeld();
}

secro::Joystick makeOctoDir(secro::Joystick j)
{
	float pi = 3.14159265359f;

	float angle = atan2(j.x, j.y);
	int octant = int(8 * angle / (2 * pi) + 8.5) % 8;
	float newAngle = octant * 45.f + 90.f;

	secro::Joystick ret;
	ret.x = -cos(newAngle / 180.f * pi);
	ret.y = sin(newAngle / 180.f * pi);
	return ret;
}

void secro::CharacterDashette::stateStartSpecial()
{
	auto stick = input->getMovement();
	stick = normalise(stick);
	stick = makeOctoDir(stick);
	stick = mul(stick, specialSpeed);

	useGravity = false;
	physicsBody->SetLinearVelocity({ stick.x, stick.y });
	setMovementState(MovementState::InAir);
	setStateTimer(specialDuration);

	//change friction
	attributes.groundDeceleration = specialGroundFriction;

	//remove a dash
	airDashLeft--;
}

void secro::CharacterDashette::stateEndSpecial()
{
	useGravity = true;
	auto vel = physicsBody->GetLinearVelocity();
	physicsBody->SetLinearVelocity(mul(vel, specialRemainSpeed));

	//reset friction
	attributes.groundDeceleration = normalFriction;
}

void secro::CharacterDashette::stateStartHyperJump()
{
	movementState = MovementState::InAir;
	
	auto vel = physicsBody->GetLinearVelocity();
	vel.y = 0.f;
	vel = mul(vel, specialHyperJumpPower);
	vel.y = -specialHyperJumpHeight;

	physicsBody->SetLinearVelocity(vel);
}
