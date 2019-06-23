#include "CharacterDashette.h"
#include "secro/framework/detail/PlainVectorMath.h"
#include "secro/framework/level/Level.h"
#include <Box2D/Box2D.h>
#include <functional>
#include <SFML/Graphics.hpp>
#include <tuple>
#include <imgui.h>
#include "secro/framework/math/fmath.h"

secro::CharacterDashette::CharacterDashette(Level* level, HitboxManager * hitboxManager, b2Body * body, std::shared_ptr<Controller> controller)
	: PlayerGraphicsCharacter(level, hitboxManager, body, controller)
{
}

secro::CharacterDashette::CharacterDashette()
{
}

void secro::CharacterDashette::init()
{
	PlayerGraphicsCharacter::init();

	specialDuration = 0.20f;
	specialSpeed = 50.f;
	specialRemainSpeed = 0.4f;
	specialGroundFriction = -100.f;
	specialHyperJumpPower = 3.f;
	specialHyperJumpHeight = 14.f;
	specialAmountOfAirDash = 3;

	normalFriction = attributes.groundDeceleration;

	dashSpeedCurve.inputMultiplicant = -1.f;
	dashSpeedCurve.inputAdd = 1.f;
	dashSpeedCurve.resultMultiplicant = -0.9f;
	dashSpeedCurve.resultAdd = 1.f;
	Circle c;
	c.exponent = 1.5f;
	dashSpeedCurve.setFormula(c);
}

void secro::CharacterDashette::loadAnimations()
{
	loadAnimation("Dashette-Run.png", 7, true, 0.06f, animRun);
	loadAnimation("Dashette-Run.png", 7, true, 0.1f, animWalk);
	loadAnimation("Dashette-groundDash.png", 1, true, 0.1f, animDash);
	loadAnimation("Dashette-base.png", 1, true, 0.1f, animStand);
	loadAnimation("Dashette-InAir.png", 1, true, 0.1f, animInAir);
	loadAnimation("Dashette-JumpSquat.png", 3, true, 0.0166666f, animJumpSquat);
	loadAnimation("Dashette-NAir.png", 4, false, 0.02f, animNAir);
	loadAnimation("Dashette-UAir.png", 7, false, 0.04f, animUAir);
	loadAnimation("Dashette-FAir.png", 2, false, 0.1f, animFAir);
	loadAnimation("Dashette-DAir.png", 2, false, 0.1f, animDAir);
	loadAnimation("Dashette-BAir.png", 2, false, 0.1f, animBAir);
	loadAnimation("Dashette-Jab.png", 5, false, 0.05f, animJab);
	loadAnimation("Dashette-DTilt.png", 5, false, 0.05f, animDTilt);
	loadAnimation("Dashette-FTilt.png", 6, false, 0.03f, animFTilt);
	loadAnimation("Dashette-UTilt.png", 7, false, 0.05f, animUTilt);
	loadAnimation("Dashette-Grab.png", 4, false, 0.05f, animGrab);
	loadAnimation("Dashette-DAir.png", 1, false, 0.05f, animHitstun);
	loadAnimation("Dashette-FreeFall.png", 1, false, 0.05f, animFreeFall);
	loadAnimation("Dashette-dash.png", 1, false, 0.03f, animSpDash);
	loadAnimation("Dashette-LandingLag.png", 1, false, 0.05f, animLandingLag);
	loadAnimation("Dashette-Shine.png", 3, false, 0.017f, animSpShine);
	loadAnimation("Dashette-TechLeft.png", 8, false, 0.03f, animTechLeft);
	loadAnimation("Dashette-TechRight.png", 8, false, 0.05f, animTechRight);
}

void secro::CharacterDashette::setupStates(StateMachine & sm)
{
	PlayerGraphicsCharacter::setupStates(sm);

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
	//sm.addCondition(PlayerState::SpecialN, PlayerState::SpecialU, [&](float f) 
	//{
	//	return getMovementState() == MovementState::OnGround && input->jumpPressed();
	//});
	//sm.addUnsetState(PlayerState::SpecialU, std::bind(&CharacterDashette::stateStartHyperJump, this));
	//sm.addCondition(PlayerState::SpecialU, PlayerState::Jump, [&](float f) 
	//{
	//	return true;
	//});

	//direction special
	sm.addSetState(PlayerState::SpecialN, std::bind(&CharacterDashette::stateStartSpecial, this));
	sm.addUnsetState(PlayerState::SpecialN, std::bind(&CharacterDashette::stateEndSpecial, this));
	sm.addCondition(PlayerState::SpecialN, PlayerState::Jump, [&](float f) 
	{
		return IsStateTimerDone();
	});
	sm.addUpdateState(PlayerState::SpecialN, std::bind(&CharacterDashette::stateUpdateSpecial, this));

	//shine
	auto condShine = [&](float f) 
	{
		auto dir = getInput()->getMovementDirection();
		return dir == Direction::Neutral && getInput()->specialPressed();
	};
	sm.addCondition(PlayerState::Stand, PlayerState::SpecialD, condShine);
	sm.addCondition(PlayerState::Dash, PlayerState::SpecialD, condShine);
	sm.addCondition(PlayerState::Run, PlayerState::SpecialD, condShine);
	sm.addCondition(PlayerState::Walk, PlayerState::SpecialD, condShine);
	sm.addCondition(PlayerState::Jump, PlayerState::SpecialD, condShine);
	sm.addCondition(PlayerState::SpecialD, PlayerState::Stand, [&](float f)
	{
		return IsStateTimerDone() && getMovementState() == MovementState::OnGround;
	});
	sm.addCondition(PlayerState::SpecialD, PlayerState::Jump, [&](float f)
	{
		return IsStateTimerDone() && getMovementState() == MovementState::InAir;
	});
	sm.addCondition(PlayerState::SpecialD, PlayerState::Jump, [&](float f) 
	{
		if (getInput()->jumpPressed() && getMovementState() == MovementState::InAir)
		{
			this->tryDoubleJump(f);
			return true;
		}
		return false;
	}); 
	sm.addCondition(PlayerState::SpecialD, PlayerState::JumpSquat, [&](float f)
	{
		return getInput()->jumpPressed() && getMovementState() == MovementState::OnGround;
	});
	sm.addSetState(PlayerState::SpecialD, std::bind(&PlayerCharacter::stateStartNewAttack, this, PlayerState::SpecialD));
	sm.addSetState(PlayerState::SpecialD, std::bind(&CharacterDashette::stateStartShineSpecial, this));
	sm.addUnsetState(PlayerState::SpecialD, std::bind(&PlayerCharacter::endAttack, this));
	sm.addUpdateState(PlayerState::SpecialD, std::bind(&CharacterDashette::stateUpdateShineSpecial, this, std::placeholders::_1));

	//animations
	sm.addSetState(PlayerState::SpecialD, [&](float f)
	{
		animatedSprite.setAnimation(animSpShine);
	});
	auto& spDir = specialDirection;
	sm.addSetState(PlayerState::SpecialN, [&](float f)
	{
		animatedSprite.setAnimation(animSpDash);
		auto newDir = spDir;
		newDir.x = -newDir.x;
		float rot = angleFromDirection(newDir);
		if (getFacingDirection() == FacingDirection::Left)
			rot = 180.f + rot;
		animatedSprite.setRotation(rot);
	});
	sm.addUnsetState(PlayerState::SpecialN, [&](float f)
	{
		animatedSprite.setRotation(0.f);
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

	atts.loadAttack("D_Jab.json", PlayerState::AJab);
	atts.loadAttack("D_UTilt.json", PlayerState::AUTilt);
	atts.loadAttack("FTilt.json", PlayerState::AFTilt);
	atts.loadAttack("DTilt.json", PlayerState::ADTilt);

	atts.loadAttack("Grab.json", PlayerState::AGrab);
	atts.loadAttack("Grab.json", PlayerState::AGrabAir);
	
	atts.loadAttack("FalShine.json", PlayerState::SpecialD);
}

void secro::CharacterDashette::update(float deltaTime)
{
	PlayerGraphicsCharacter::update(deltaTime);

	if (movementState == MovementState::OnGround)
	{
		airDashLeft = specialAmountOfAirDash;
	}
}

void secro::CharacterDashette::render(sf::RenderWindow & window)
{
	PlayerGraphicsCharacter::render(window);
}

void secro::CharacterDashette::renderAttributes(sf::RenderWindow & window)
{
	ImGui::Separator();

	ImGui::InputFloat("Special Dash Speed", &specialSpeed);
	dashSpeedCurve.renderCurveEditor();
}

int secro::CharacterDashette::getAirDashLeft()
{
	return airDashLeft;
}

bool secro::CharacterDashette::canBeginSpecial(float dt)
{
	auto dir = input->getMovementDirection();
	if (dir == Direction::Neutral)
		return false;

	auto joy = input->getMovement();
	return input->specialPressed() && getAirDashLeft() > 0 && length(joy) > 0.2f;
}

bool secro::CharacterDashette::canBeginSpecialFromAttack(float dt)
{
	auto dir = input->getMovementDirection();
	if (dir == Direction::Neutral)
		return false;

	auto joy = input->getMovement();
	return getHasAttackHit() && input->specialHeld() && length(joy) > 0.2f;
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

	//set between value
	specialDirection = { -stick.x, stick.y };

	stick = mul(stick, specialSpeed);


	useGravity = false;
	physicsBody->SetLinearVelocity({ stick.x, stick.y });
	setMovementState(MovementState::InAir);
	setStateTimer(specialDuration);

	//change friction
	//attributes.groundDeceleration = specialGroundFriction;

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

void secro::CharacterDashette::stateUpdateSpecial()
{
	float alpha = getStateTimer() / specialDuration;
	alpha = 1.f - alpha;
	alpha = clampOne(alpha);
	resizeVelocity(dashSpeedCurve.calculate(alpha) * specialSpeed);
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

void secro::CharacterDashette::stateStartShineSpecial()
{
	auto vel = physicsBody->GetLinearVelocity();
	vel.y /= 3.f;

	if (movementState == MovementState::OnGround)
	{
		vel.x /= 1.5f;
	}
	else
	{
		vel.x /= 3.f;
	}

	physicsBody->SetLinearVelocity(vel);
	setStateTimer(0.2f);
}

void secro::CharacterDashette::stateUpdateShineSpecial(float deltaTime)
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