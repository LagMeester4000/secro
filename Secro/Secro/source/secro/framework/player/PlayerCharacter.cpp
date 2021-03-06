#include "PlayerCharacter.h"
#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include "../input/Controller.h"
#include "../collision/HitboxCollection.h"
#include "../collision/HitboxManager.h"
#include "../physics/Filters.h"
#include "../detail/PlainVectorMath.h"
#include "../DebugOptions.h"
#include "../GameplaySettings.h"
#include "secro/framework/level/Level.h"
#include "secro/framework/math/fmath.h"

using namespace secro;

secro::PlayerCharacter::PlayerCharacter()
{
}

secro::PlayerCharacter::PlayerCharacter(Level* levell, HitboxManager* hitboxM, PhysicsHandle body, std::shared_ptr<Controller> controller)
{
	hitboxManager = hitboxM;
	physicsBody = body;
	input = controller;
	facingDirection = FacingDirection::Right;
	level = levell;
}

void secro::PlayerCharacter::lateSetup(Level* levell, HitboxManager * hitboxM, PhysicsHandle body, std::shared_ptr<Controller> controller)
{
	hitboxManager = hitboxM;
	physicsBody = body;
	input = controller;
	facingDirection = FacingDirection::Right;
	level = levell;
}

void secro::PlayerCharacter::init()
{
	//walking deadzone
	walkDeadzone = 30.f;

	//setup testing attributes
	auto& a = attributes;
	a.airAcceleration = 40.f;
	a.airDeceleration = 10.f;
	a.airMaxSpeed = 8.f;
	//dash
	a.dashDuration = 0.25f;
	a.dashInitialSpeed = 11.5f;
	a.dashSpeedCurve.setFormula(Linear{});
	a.dashSpeedCurve.resultMultiplicant = 3.f;
	//fall
	a.doubleJumpSpeed = 20.f;
	a.fallSpeed = 50.f;
	a.fastfallSpeed = 10.f;
	a.groundDeceleration = 40.f;
	a.jumpAmount = 1;
	a.jumpFullSpeed = 17.f;
	a.jumpShortSpeed = 10.f;
	a.jumpSquatDuration = 0.05f;
	//run
	a.runAcceleration = 50.f;
	a.runMaxSpeed = 8.f;
	a.walkMaxSpeed = 6.f;
	//airdodge
	a.airdodgeDuration = 0.25f;
	a.airdodgeInvDuration = 0.21f;
	a.airdodgeInvStart = 0.04f;
	a.airdodgeLandingLag = 0.1f;
	a.airdodgeSpeed = 15.f;
	Circle c;
	c.exponent = 1.5f;
	a.airdodgeSpeedCurve.setFormula(c);
	a.airdodgeSpeedCurve.resultAdd = 1.f;
	a.airdodgeSpeedCurve.resultMultiplicant = -0.7f;
	a.airdodgeSpeedCurve.inputAdd = 1.f;
	a.airdodgeSpeedCurve.inputMultiplicant = -1.f;
	//tech
	a.techInPlaceDuration = 0.2f;
	a.techInPlaceInvDuration = 0.14f;
	a.techRollDuration = 0.2f;
	a.techRollInvDuration = 0.17f;
	a.techRollSpeed = 8.f;
	//airdash
	a.airDashCooldown = 1.f;
	a.airDashSpeed = 10.f;
	a.airDashInputTimeFrame = 0.5f;
		
	state = PlayerState::Jump;
	movementState = MovementState::InAir;

	//call state machine setup
	setupStates(stateMachine);

	//setup the attacks
	setupAttacks(attackCollection);

	//set up hurtbox
	hurtboxFrames = attackCollection.loadRaw("Hurt.json");
	hurtbox = hitboxManager->addHurtbox(this, hurtboxFrames);
}

void secro::PlayerCharacter::setupStates(StateMachine & sm)
{
	//standing
	sm.addUnsetState(PlayerState::Stand, std::bind(&PlayerCharacter::stateEndStand, this));
	sm.addUpdateState(PlayerState::Stand, std::bind(&PlayerCharacter::stateUpdateStand, this, std::placeholders::_1));

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
	sm.addUnsetState(PlayerState::Dash, std::bind(&PlayerCharacter::stateEndDash, this));
	sm.addCondition(PlayerState::Dash, PlayerState::Stand, [&](float f)
	{
		auto input = getInput();
		auto mov = input->getMovementDirection();
		auto smash = input->getMovementPushDirectionExt();
		auto face = getFacingDirection();
		bool ret = smash.speed > 30.f && ((mov == Direction::Left && face == FacingDirection::Right) || (mov == Direction::Right && face == FacingDirection::Left));
		if (ret)
		{
			auto& coll = getPhysicsCollider();
			auto vel = coll.getVelocity();
			vel.x = 0.f;
			coll.setVelocity(vel);
			return true;
		}
		return false;
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
		auto smash = input->getMovementPushDirectionExt(true);
		auto mov = input->getMovementDirection();
		return smash.speed > 20.f && ((mov == Direction::Left) || (mov == Direction::Right));
	});
	sm.addCondition(PlayerState::Dash, PlayerState::Stand, [&](float f)
	{
		return IsStateTimerDone()/* && getPhysicsBody()->GetLinearVelocity().Length() < 1.f*/;
	});
	sm.addUpdateState(PlayerState::Dash, std::bind(&PlayerCharacter::stateUpdateDash, this, std::placeholders::_1));


	//run
	sm.addCondition(PlayerState::Run, PlayerState::Stand, [&](float f)
	{
		return length(getPhysicsCollider().getVelocity()) < 0.1f;
	});
	sm.addCondition(PlayerState::Run, PlayerState::Dash, [&](float f)
	{
		auto input = getInput();
		auto mov = input->getMovementDirection();
		auto smash = input->getMovementPushDirectionExt();
		auto face = getFacingDirection();
		return smash.speed > 30.f && ((mov == Direction::Left && face == FacingDirection::Right) || (mov == Direction::Right && face == FacingDirection::Left));
	});


	//walk
	sm.addCondition(PlayerState::Stand, PlayerState::Walk, [&](float f)
	{
		auto input = getInput();
		auto smash = input->getMovementPushDirectionExt();
		auto mov = input->getMovementDirection();
		auto joy = input->getMovement();
		bool oldCond = smash.speed <= 30.f && ((mov == Direction::Left) || (mov == Direction::Right));
		bool upDown = (mov != Direction::Up && mov != Direction::Down);
		return /*oldCond ||*/ (upDown && abs(joy.x) > getWalkDeadzone() && smash.speed <= 30.f);
	});
	sm.addCondition(PlayerState::Walk, PlayerState::Stand, [&](float f)
	{
		//auto input = getInput();
		//auto mov = input->getMovementDirection();
		//return !isEqual(getFacingDirection(), mov);
		return !keepRunning();
	});
	sm.addCondition(PlayerState::Walk, PlayerState::Dash, [&](float f)
	{
		auto input = getInput();
		auto mov = input->getMovementDirection();
		auto smash = input->getMovementPushDirectionExt();
		auto face = getFacingDirection();
		return smash.speed > 20.f && (mov == Direction::Left || mov == Direction::Right); // && ((mov == Direction::Left && face == FacingDirection::Right) || (mov == Direction::Right && face == FacingDirection::Left));
	});
	sm.addSetState(PlayerState::Walk, std::bind(&PlayerCharacter::stateStartWalk, this));


	//attacks
	auto endAerialA = [&](float f)
	{
		return getMovementState() == MovementState::OnGround;
	};
	auto endAerialAir = [&](float f)
	{
		return getMovementState() == MovementState::InAir && IsStateTimerDone();
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
	sm.addCondition(PlayerState::ANAir, PlayerState::Jump, endAerialAir);
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
	sm.addCondition(PlayerState::AUAir, PlayerState::Jump, endAerialAir);
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
	sm.addCondition(PlayerState::AFAir, PlayerState::Jump, endAerialAir);
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
	sm.addCondition(PlayerState::ABAir, PlayerState::Jump, endAerialAir);
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
	sm.addCondition(PlayerState::ADAir, PlayerState::Jump, endAerialAir);

	//ground attacks
	auto endGroundAttack = [&](float f) {
		return IsStateTimerDone();
	};
	//jab
	auto JabAttack = [&](float f)
	{
		auto input = getInput();
		auto dir = input->getMovementDirection();
		auto aDir = input->getDirAttackDirection();
		return (dir == Direction::Neutral && input->attackPressed());
	};
	sm.addCondition(PlayerState::Stand, PlayerState::AJab, JabAttack);
	sm.addCondition(PlayerState::Walk, PlayerState::AJab, JabAttack);
	sm.addCondition(PlayerState::Run, PlayerState::AJab, JabAttack);
	sm.addSetState(PlayerState::AJab, std::bind(&PlayerCharacter::stateStartNewAttack, this, PlayerState::AJab));
	sm.addUnsetState(PlayerState::AJab, std::bind(&PlayerCharacter::endAttack, this));
	sm.addCondition(PlayerState::AJab, PlayerState::Stand, endGroundAttack);
	//utilt
	auto UTiltAttack = [&](float f)
	{
		auto input = getInput();
		auto dir = input->getMovementDirection();
		auto aDir = input->getDirAttackDirection();
		return (dir == Direction::Up && input->attackPressed()) || aDir == Direction::Up;
	};
	sm.addCondition(PlayerState::Stand, PlayerState::AUTilt, UTiltAttack);
	sm.addCondition(PlayerState::Walk, PlayerState::AUTilt, UTiltAttack);
	sm.addCondition(PlayerState::Run, PlayerState::AUTilt, UTiltAttack);
	sm.addCondition(PlayerState::Dash, PlayerState::AUTilt, UTiltAttack);
	sm.addSetState(PlayerState::AUTilt, std::bind(&PlayerCharacter::stateStartNewAttack, this, PlayerState::AUTilt));
	sm.addUnsetState(PlayerState::AUTilt, std::bind(&PlayerCharacter::endAttack, this));
	sm.addCondition(PlayerState::AUTilt, PlayerState::Stand, endGroundAttack);
	//ftilt
	auto& facingRef = facingDirection;
	auto FTiltAttack = [&](float f)
	{
		auto input = getInput();
		auto dir = input->getMovementDirection();
		auto aDir = input->getDirAttackDirection();
		if (isOpposite(getFacingDirection(), aDir))
		{
			if (facingDirection == FacingDirection::Left)
			{
				facingDirection = FacingDirection::Right;
			}
			else if (facingDirection == FacingDirection::Right)
			{
				facingDirection = FacingDirection::Left;
			}
		}
		return (isEqual(getFacingDirection(), dir) && input->attackPressed()) || isEqual(getFacingDirection(), aDir) || isOpposite(getFacingDirection(), aDir);
	};
	sm.addCondition(PlayerState::Stand, PlayerState::AFTilt, FTiltAttack);
	sm.addCondition(PlayerState::Walk, PlayerState::AFTilt, FTiltAttack);
	sm.addCondition(PlayerState::Run, PlayerState::AFTilt, FTiltAttack);
	sm.addCondition(PlayerState::Dash, PlayerState::AFTilt, FTiltAttack);
	sm.addSetState(PlayerState::AFTilt, std::bind(&PlayerCharacter::stateStartNewAttack, this, PlayerState::AFTilt));
	sm.addUnsetState(PlayerState::AFTilt, std::bind(&PlayerCharacter::endAttack, this));
	sm.addCondition(PlayerState::AFTilt, PlayerState::Stand, endGroundAttack);
	//dtilt
	auto DTiltAttack = [&](float f)
	{
		auto input = getInput();
		auto dir = input->getMovementDirection();
		auto aDir = input->getDirAttackDirection();
		return (dir == Direction::Down && input->attackPressed()) || aDir == Direction::Down;
	};
	sm.addCondition(PlayerState::Stand, PlayerState::ADTilt, DTiltAttack);
	sm.addCondition(PlayerState::Walk, PlayerState::ADTilt, DTiltAttack);
	sm.addCondition(PlayerState::Run, PlayerState::ADTilt, DTiltAttack);
	sm.addCondition(PlayerState::Dash, PlayerState::ADTilt, DTiltAttack);
	sm.addSetState(PlayerState::ADTilt, std::bind(&PlayerCharacter::stateStartNewAttack, this, PlayerState::ADTilt));
	sm.addUnsetState(PlayerState::ADTilt, std::bind(&PlayerCharacter::endAttack, this));
	sm.addCondition(PlayerState::ADTilt, PlayerState::Stand, endGroundAttack);

	//grabs
	//ground
	auto GrabAttack = [&](float f)
	{
		auto input = getInput();
		return input->grabPressed();
	};
	sm.addCondition(PlayerState::Stand, PlayerState::AGrab, GrabAttack);
	sm.addCondition(PlayerState::Walk, PlayerState::AGrab, GrabAttack);
	sm.addCondition(PlayerState::Run, PlayerState::AGrab, GrabAttack);
	sm.addCondition(PlayerState::Dash, PlayerState::AGrab, GrabAttack);
	sm.addCondition(PlayerState::Shield, PlayerState::AGrab, GrabAttack);
	sm.addCondition(PlayerState::Shield, PlayerState::AGrab, [&](float f) 
	{
		return getInput()->attackPressed();
	});
	sm.addSetState(PlayerState::AGrab, std::bind(&PlayerCharacter::stateStartNewAttack, this, PlayerState::AGrab));
	sm.addUnsetState(PlayerState::AGrab, std::bind(&PlayerCharacter::endAttack, this));
	sm.addCondition(PlayerState::AGrab, PlayerState::Stand, endGroundAttack);
	//air
	sm.addCondition(PlayerState::Jump, PlayerState::AGrabAir, [&](float f)
	{
		auto input = getInput();
		return input->grabPressed();
	});
	sm.addSetState(PlayerState::AGrabAir, std::bind(&PlayerCharacter::stateStartNewAttack, this, PlayerState::AGrabAir));
	sm.addUnsetState(PlayerState::AGrabAir, std::bind(&PlayerCharacter::endAttack, this));
	sm.addCondition(PlayerState::AGrabAir, PlayerState::LandingLag, endAerialA);
	sm.addCondition(PlayerState::AGrabAir, PlayerState::Jump, endAerialAir);


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
		return !isInHitstun();
	});


	//shielding
	auto shieldCond = [&](float f)
	{
		return getInput()->blockHeld();
	};
	sm.addCondition(PlayerState::Stand, PlayerState::Shield, shieldCond);
	sm.addCondition(PlayerState::Run, PlayerState::Shield, shieldCond);
	sm.addCondition(PlayerState::Dash, PlayerState::Shield, shieldCond);
	sm.addCondition(PlayerState::Walk, PlayerState::Shield, shieldCond);
	sm.addCondition(PlayerState::Jump, PlayerState::Shield, [&](float f)
	{
		return getInput()->blockHeld() && getInput()->getMovementDirection() == Direction::Neutral;
	});
	sm.addCondition(PlayerState::Shield, PlayerState::Stand, [&](float f)
	{
		return !getInput()->blockHeld() && getMovementState() == MovementState::OnGround;
	});
	sm.addCondition(PlayerState::Shield, PlayerState::Jump, [&](float f)
	{
		return !getInput()->blockHeld() && getMovementState() == MovementState::InAir;
	});
	//activation and deactivation
	sm.addSetState(PlayerState::Shield, std::bind(&PlayerCharacter::stateStartShield, this));
	sm.addUnsetState(PlayerState::Shield, std::bind(&PlayerCharacter::stateEndShield, this));


	//special fall
	sm.addCondition(PlayerState::SpecialFall, PlayerState::Stand, [&](float f) 
	{
		return getMovementState() == MovementState::OnGround;
	});


	//airdodge
	sm.addCondition(PlayerState::Jump, PlayerState::Airdodge, std::bind(&PlayerCharacter::stateCanAirdodge, this));
	//sm.addCondition(PlayerState::JumpSquat, PlayerState::Airdodge, std::bind(&PlayerCharacter::stateCanEarlyAirdodge, this));
	sm.addCondition(PlayerState::Airdodge, PlayerState::SpecialFall, [&](float f) 
	{
		return IsStateTimerDone();
	});
	sm.addCondition(PlayerState::Airdodge, PlayerState::LandingLag, [&](float f)
	{
		return getMovementState() == MovementState::OnGround;
	});
	//activation/deactivation
	sm.addSetState(PlayerState::Airdodge, std::bind(&PlayerCharacter::stateAirdodgeStart, this));
	sm.addUnsetState(PlayerState::Airdodge, std::bind(&PlayerCharacter::stateAirdodgeEnd, this));
	sm.addUpdateState(PlayerState::Airdodge, std::bind(&PlayerCharacter::stateUpdateAirdodge, this, std::placeholders::_1));


	//teching
	sm.addCondition(PlayerState::Hitstun, PlayerState::TechInPlace, [&](float f) 
	{
		auto dir = getInput()->getMovementDirection();
		return stateCanTech() &&
			dir != Direction::Left && dir != Direction::Right;
	});
	sm.addCondition(PlayerState::Hitstun, PlayerState::TechRollRight, [&](float f)
	{
		auto dir = getInput()->getMovementDirection();
		return stateCanTech() &&
			dir == Direction::Right;
	});
	sm.addCondition(PlayerState::Hitstun, PlayerState::TechRollLeft, [&](float f)
	{
		auto dir = getInput()->getMovementDirection();
		return stateCanTech() &&
			dir == Direction::Left;
	});
	sm.addCondition(PlayerState::TechInPlace, PlayerState::Stand, std::bind(&PlayerCharacter::stateCanTechEnd, this));
	sm.addCondition(PlayerState::TechRollLeft, PlayerState::Stand, std::bind(&PlayerCharacter::stateCanTechEnd, this));
	sm.addCondition(PlayerState::TechRollRight, PlayerState::Stand, std::bind(&PlayerCharacter::stateCanTechEnd, this));
	sm.addSetState(PlayerState::TechRollLeft, std::bind(&PlayerCharacter::stateTechLeftBegin, this));
	sm.addSetState(PlayerState::TechRollRight, std::bind(&PlayerCharacter::stateTechRightBegin, this));
	sm.addSetState(PlayerState::TechInPlace, std::bind(&PlayerCharacter::stateTechInPlaceBegin, this));
	sm.addUnsetState(PlayerState::TechRollLeft, std::bind(&PlayerCharacter::stateTechEnd, this));
	sm.addUnsetState(PlayerState::TechRollRight, std::bind(&PlayerCharacter::stateTechEnd, this));
	sm.addUnsetState(PlayerState::TechInPlace, std::bind(&PlayerCharacter::stateTechEnd, this));



	sm.addUpdateState(PlayerState::Jump, std::bind(&PlayerCharacter::conditionAirDash, this));
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

	atts.loadAttack("Grab.json", PlayerState::AGrab);
	atts.loadAttack("Grab.json", PlayerState::AGrabAir);
}

void secro::PlayerCharacter::update(float deltaTime)
{
	updateDI(deltaTime);
	updateHitlag(deltaTime);
	updateAirDashTimer(deltaTime);
	updatePlatformCollision();

	if (!isInHitlag())
	{
		updateMovement(deltaTime);
		updateHitstun(deltaTime);
		updateState(deltaTime);
		updateAttack(deltaTime);
		updateInvincibility(deltaTime);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
	{
		damage = 0.f;
	}
}

void secro::PlayerCharacter::render(sf::RenderWindow & window)
{
	//draw attributes gui
	if (DebugOptions::getOptions().enablePlayerAttributeEditor)
		debugRenderAttributes(window);

	if (!DebugOptions::getOptions().enablePhysicsRender)
		return;

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
	else if (state == PlayerState::Walk)
	{
		c.setFillColor(sf::Color(0, 255, 255));
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


	//draw shield
	if (state == PlayerState::Shield)
	{
		sf::CircleShape shield(1.f);
		shield.setPosition(convNR<sf::Vector2f>(getPosition()) - sf::Vector2f(1.f, 1.f));
		shield.setFillColor(sf::Color(0, 100, 255, 126));
		window.draw(shield);
	}


	//draw invincibility thingy
	if (isInvincible())
	{
		sf::CircleShape inv(0.6f);
		inv.setFillColor(sf::Color(0, 0, 200, 126));
		inv.setPosition(convNR<sf::Vector2f>(getPosition()) - sf::Vector2f(0.6f, 0.6f));
		window.draw(inv);
	}
}

void secro::PlayerCharacter::netSerSave(RawSerializeBuffer & buff)
{
	Entity::netSerSave(buff);

	buff.save(movementState);
	buff.save(previousMovementState);
	buff.save(attributes);
	buff.save(facingDirection);
	buff.save(jumpsLeft);
	buff.save(useGravity);
	buff.save(canSlideOffPlatforms);
	buff.save(previousPosition);
	buff.save(shouldHaveFriction);
	buff.save(walkDeadzone);
	buff.save(hitstun);
	buff.save(state);
	buff.save(stateTimer);
	buff.save(previousStateTimer);
	buff.save(damage);
	buff.save(debugDamage);
	buff.save(attackTimer);
	buff.save(currentAttackState);
	buff.save(currentAttackHitbox);
	buff.save(hurtbox);
	buff.save(hasAttackHit);
	buff.save(hitlag);
	buff.save(hitlagVelocity);
	buff.save(freezeShake);
	buff.save(freezeShakePerHitlag);
	buff.save(freezeShakeBase);
	buff.save(airdodgeDirectrion);
	buff.save(invincibilityTimer);
	buff.save(DI);
	buff.save(airDashTimer);
	buff.save(airDashPreviousDirection);
	buff.save(airDashDirection);
	buff.save(canAirDash);
	buff.save(isAirDashUsed);
}

void secro::PlayerCharacter::netSerLoad(RawSerializeBuffer & buff)
{
	Entity::netSerLoad(buff);

	buff.load(movementState);
	buff.load(previousMovementState);
	buff.load(attributes);
	buff.load(facingDirection);
	buff.load(jumpsLeft);
	buff.load(useGravity);
	buff.load(canSlideOffPlatforms);
	buff.load(previousPosition);
	buff.load(shouldHaveFriction);
	buff.load(walkDeadzone);
	buff.load(hitstun);
	buff.load(state);
	buff.load(stateTimer);
	buff.load(previousStateTimer);
	buff.load(damage);
	buff.load(debugDamage);
	buff.load(attackTimer);
	buff.load(currentAttackState);
	buff.load(currentAttackHitbox);
	buff.load(hurtbox);
	buff.load(hasAttackHit);
	buff.load(hitlag);
	buff.load(hitlagVelocity);
	buff.load(freezeShake);
	buff.load(freezeShakePerHitlag);
	buff.load(freezeShakeBase);
	buff.load(airdodgeDirectrion);
	buff.load(invincibilityTimer);
	buff.load(DI);
	buff.load(airDashTimer);
	buff.load(airDashPreviousDirection);
	buff.load(airDashDirection);
	buff.load(canAirDash);
	buff.load(isAirDashUsed);
}

Vector2 secro::PlayerCharacter::getPosition()
{
	return level->getPhysics().getCollider(physicsBody).getPosition();
}

void secro::PlayerCharacter::setPosition(Vector2 pos, bool resetVelocity)
{
	auto& coll = level->getPhysics().getCollider(physicsBody);
	coll.setPosition(pos);
	if (resetVelocity)
		coll.setVelocity({ 0.f, 0.f });
}

void secro::PlayerCharacter::reset(Vector2 position)
{
	auto& coll = level->getPhysics().getCollider(physicsBody);
	damage = debugDamage;
	coll.setPosition(position);
	coll.setVelocity({ 0.f, 0.f });
}

void secro::PlayerCharacter::freeze()
{
	auto& coll = level->getPhysics().getCollider(physicsBody);
	coll.setVelocity({ 0.f, 0.f });
}

void secro::PlayerCharacter::updateMovement(float deltaTime)
{
	auto& coll = level->getPhysics().getCollider(physicsBody);

	//swap the movementstate
	previousMovementState = movementState;

	switch (movementState)
	{
	case MovementState::InAir:
	{
		//update gravity
		if (useGravity)
		{
			auto cvel = coll.getVelocity();
			coll.setVelocity(Vector2{ cvel.x, cvel.y + attributes.fallSpeed * deltaTime/* * deltaTime*/ });
		}

		//actual movement
		if (!isInHitstun())
		{
			//double jumps
			if (state == PlayerState::Jump)
			{
				if (input->jumpPressed() && jumpsLeft > 0)
				{
					jumpsLeft--;
					Vector2 jumpVel = { 0.f, -attributes.doubleJumpSpeed };
					coll.setVelocity(jumpVel);
					onDoubleJump();
				}
			}

			//fastfall
			if (input->getMovementPushDirection() == Direction::Down)
			{
				auto vel = coll.getVelocity();
				if (vel.y > 0.f && vel.y < attributes.fastfallSpeed)
				{
					coll.setVelocity({ vel.x, attributes.fastfallSpeed });
				}
			}

			auto mdir = input->getMovementDirection();
			if (mdir != Direction::Neutral)
			{
				if (mdir == Direction::Left)
				{
					//maybe setting velocity wrong !!!
					auto vel = coll.getVelocity();

					Vector2 unitVec = { -attributes.airAcceleration * deltaTime/* * deltaTime*/, 0.f };

					if (vel.x > -attributes.airMaxSpeed)
					{
						vel += unitVec;
						coll.setVelocity(vel);
					}
				}
				else if (mdir == Direction::Right)
				{
					//maybe setting velocity wrong !!!
					auto vel = coll.getVelocity();

					Vector2 unitVec = { attributes.airAcceleration * deltaTime/* * deltaTime*/, 0.f };

					if (vel.x < attributes.airMaxSpeed)
					{
						vel += unitVec;
						coll.setVelocity(vel);
					}
				}

			}

			//apply tracktion when needed
			if (shouldHaveFriction)
			{
				auto vel = coll.getVelocity();
				//if (input->getMovementDirection() == Direction::Neutral)
				if (abs(vel.x) >= attributes.airMaxSpeed || !keepRunning() || state != PlayerState::Hitstun)
				{
					auto decelVal = attributes.airDeceleration * deltaTime/* * deltaTime*/;

					if (abs(vel.x) > decelVal)
					{
						auto adder = vel.x;
						adder > 0 ? adder = 1.f : adder = -1.f;
						adder *= decelVal;
						vel.x -= adder;
					}
					else
					{
						vel = { 0.f, vel.y };
					}

					coll.setVelocity(vel);
				}
			}
		}

		//snap to ground
		auto FVel = coll.getVelocity();
		if ((!isInHitstun() || !isInHitlag()) && FVel.y > 0.f)
		{
			if (snapToGround(FVel.y * deltaTime * 1.2f, isAttacking()))
			{
				movementState = MovementState::OnGround;
				FVel.y = 0.f;
				coll.setVelocity(FVel);
			}
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
					auto vel = coll.getVelocity();
					
					Vector2 unitVec = { -attributes.runAcceleration * deltaTime/* * deltaTime*/, 0.f };

					if (abs(vel.x) < maxSpeed)
					{
						vel += unitVec;
						coll.setVelocity(vel);
					}
				}
				else
				{
					//maybe setting velocity wrong !!!
					auto vel = coll.getVelocity();

					Vector2 unitVec = { attributes.runAcceleration * deltaTime/* * deltaTime*/, 0.f };

					if (abs(vel.x) < maxSpeed)
					{
						vel += unitVec;
						coll.setVelocity(vel);
					}
				}

			}
		}

		//apply tracktion when needed
		if (shouldHaveFriction)
		{
			if ((!keepRunning() || groundSpeedTooHigh())/* && state != PlayerState::Dash*/)
			{
				auto decelVal = attributes.groundDeceleration * deltaTime/* * deltaTime*/;
				auto vel = coll.getVelocity();

				if (length(vel) > decelVal)
				{
					auto adder = vel;
					adder = normalise(adder);
					adder *= decelVal;
					vel -= adder;
				}
				else
				{
					vel = { 0.f, 0.f };
				}

				coll.setVelocity(vel);
			}
		}

		//shap to ground
		if (!snapToGround(0.3f))
		{
			if (canSlideOffPlatforms)
			{
				movementState = MovementState::InAir;
				stateMachine.changeState(this, PlayerState::Jump, deltaTime);
			}
			else //put the character back on the platform 
			{
				coll.setPosition(previousPosition);
				auto oldVel = coll.getVelocity();
				coll.setVelocity({ 0.f, oldVel.y });
			}
		}
	} break;
	}


	//update the previous position
	previousPosition = coll.getPosition();
}

void secro::PlayerCharacter::updatePlatformCollision()
{
	if (canDropThroughPlatform())
	{
		getPhysicsCollider().setProfile(ColliderProfile::EntityNoPlatform);
	}
	else
	{
		getPhysicsCollider().setProfile(ColliderProfile::Entity);
	}
}

//NEEDS A FULL REWRITE
bool secro::PlayerCharacter::snapToGround(float distance, bool startAtBottom)
{
	return level->getPhysics().pushDown(physicsBody, 0.01f);
	/*auto& coll = level->getPhysics().getCollider(physicsBody);
	auto vel = coll.getVelocity();
	
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
				//ignore players but don't ignore platforms
				if (fixture->GetFilterData().groupIndex == GROUP_PLAYER && fixture->GetFilterData().categoryBits != CATEGORY_STAGE_PLATFORM)
					return -1;

				if (fixture->GetFilterData().categoryBits == CATEGORY_STAGE_PLATFORM && ignorePlatforms)
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
		bool ignorePlatforms = false;
	};

	auto* world = physicsBody->GetWorld();

	auto callBack = IgnorePlayerRay();
	
	//see if the player can drop through platforms
	callBack.ignorePlatforms = canDropThroughPlatform();

	b2Vec2 backDist = { 0.f, 0.8f };
	if (startAtBottom)
		backDist.y = 1.f;

	b2Vec2 p1 = getPosition() + backDist;
	b2Vec2 p2 = p1 + b2Vec2{ 0.f, distance + 0.2f };

	float d = secro::distance(p1, p2);
	//ERROR, it is -nan(ind)
	if (d == 0.f || d == NAN || d == -NAN || d == INFINITY || d == -INFINITY)
		return false;

	world->RayCast(&callBack, p1, p2);

	if (callBack.m_fixture != nullptr)
	{
		b2Vec2 oldPos = physicsBody->GetPosition();
		b2Vec2 newPos = b2Vec2{ oldPos.x, callBack.m_point.y - 1.f };
		physicsBody->SetTransform(newPos, 0.f);
		return true;
	}
	return false;*/

	return false;
}

//void secro::PlayerCharacter::resizeVelocity(float newSize)
//{
//	auto vel = physicsBody->GetLinearVelocity();
//	vel.Normalize();
//	vel *= newSize;
//	physicsBody->SetLinearVelocity(vel);
//}

bool secro::PlayerCharacter::keepRunning()
{
	auto joy = input->getMovement();
	if (abs(joy.x) < walkDeadzone)
		return false;

	if (state == PlayerState::Stand)
		return false;

	FacingDirection face;
	if (joy.x > 0.f)
		face = FacingDirection::Right;
	else
		face = FacingDirection::Left;

	return face == facingDirection && (state == PlayerState::Walk || state == PlayerState::Run || state == PlayerState::Dash);

	//return isEqual(facingDirection, input->getMovementDirection()) && (state == PlayerState::Walk || state == PlayerState::Run || state == PlayerState::Dash);
}

void secro::PlayerCharacter::stateEndStand()
{
	canSlideOffPlatforms = true;
}

void secro::PlayerCharacter::stateStartJump()
{
	auto& coll = level->getPhysics().getCollider(physicsBody);
	if (state == PlayerState::Airdodge)
		return;

	movementState = MovementState::InAir;

	bool fullHop = input->jumpHeld();

	auto currentVel = coll.getVelocity();
	if (fullHop)
		coll.setVelocity({ currentVel.x, -attributes.jumpFullSpeed });
	else
		coll.setVelocity({ currentVel.x, -attributes.jumpShortSpeed });
}

void secro::PlayerCharacter::stateStartJumpSquat()
{
	stateTimer = attributes.jumpSquatDuration;
}

void secro::PlayerCharacter::stateStartDash()
{
	auto& coll = level->getPhysics().getCollider(physicsBody);
	auto dir = input->getMovementDirection();
	
	if (dir == Direction::Left)
	{
		facingDirection = FacingDirection::Left;
		coll.setVelocity(Vector2{ -attributes.dashInitialSpeed, 0.f });
	}
	else if (dir == Direction::Right)
	{
		facingDirection = FacingDirection::Right;
		coll.setVelocity(Vector2{ attributes.dashInitialSpeed, 0.f });
	}

	stateTimer = attributes.dashDuration;
}

void secro::PlayerCharacter::stateEndDash()
{
	canSlideOffPlatforms = true;
}

void secro::PlayerCharacter::stateStartWalk()
{
	auto dir = input->getMovement();

	if (dir.x < 0.f)
	{
		facingDirection = FacingDirection::Left;
	}
	else if (dir.x >= 0.f)
	{
		facingDirection = FacingDirection::Right;
	}
}

void secro::PlayerCharacter::endAttack()
{
	//destroy and reset attack
	if (currentAttackHitbox != HitboxId::None)
	{
		hitboxManager->getHitbox(currentAttackHitbox)->destroy();
	}
	currentAttackHitbox = HitboxId::None;

	if (movementState == MovementState::OnGround)
	{
		auto& at = attackCollection.getAttack(currentAttackState);
		stateTimer = at.landingLag;
	}

	//reset has attack hit
	hasAttackHit = false;
}

bool secro::PlayerCharacter::groundSpeedTooHigh()
{
	auto& coll = level->getPhysics().getCollider(physicsBody);
	auto vel = coll.getVelocity();
	
	if (state == PlayerState::Run)
		return length(vel) > attributes.runMaxSpeed;
	else if (state == PlayerState::Walk)
		return length(vel) > attributes.walkMaxSpeed;

	return false;
}

bool secro::PlayerCharacter::canDropThroughPlatform()
{
	if (state == PlayerState::Airdodge)
		return false;

	if (movementState == MovementState::InAir)
	{
		if (isAttacking() || isInHitstun())
			return false;
		return input->getMovementDirection() == Direction::Down;
	}
	else if (movementState == MovementState::OnGround)
	{
		if (isAttacking() || isInHitstun() || state == PlayerState::JumpSquat || state == PlayerState::LandingLag)
			return false;

		for (size_t i = 0; i < 5; ++i)
		{
			if (input->getDirection(input->getInput(i).leftStick) != Direction::Down)
				return false;
		}

		return true;
	}
	return false;
}

void secro::PlayerCharacter::debugRenderAttributes(sf::RenderWindow & window)
{
	ImGui::PushID(this);
	std::string name = "Player Movement Attributes" + std::to_string((long long int)this);
	if (ImGui::Begin(name.c_str()))
	{
		//TEMP
		ImGui::Text("TEMP debug values");
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

		ImGui::Text("Airdodge");
		ImGui::InputFloat("AirdodgeInvStart", &attributes.airdodgeInvStart);
		ImGui::InputFloat("AirdodgeInvDuration", &attributes.airdodgeInvDuration);
		ImGui::InputFloat("AirdodgeDuration", &attributes.airdodgeDuration);
		ImGui::InputFloat("AirdodgeSpeed", &attributes.airdodgeSpeed);
		ImGui::InputFloat("AirdodgeLandingLag", &attributes.airdodgeLandingLag);
		attributes.airdodgeSpeedCurve.renderCurveEditor();

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
		attributes.dashSpeedCurve.renderCurveEditor();

		ImGui::Separator();

		ImGui::Text("Ground Movement");
		ImGui::InputFloat("RunAcceleration", &attributes.runAcceleration);
		ImGui::InputFloat("WalkMaxSpeed", &attributes.walkMaxSpeed);
		ImGui::InputFloat("RunMaxSpeed", &attributes.runMaxSpeed);
		ImGui::InputFloat("GroundDeceleration", &attributes.groundDeceleration);

		ImGui::Separator();

		ImGui::Text("Teching");
		ImGui::InputFloat("TechInPlaceInvDuration", &attributes.techInPlaceInvDuration);
		ImGui::InputFloat("TechInPlaceDuration", &attributes.techInPlaceDuration);
		ImGui::InputFloat("TechRollInvDuration", &attributes.techRollInvDuration);
		ImGui::InputFloat("TechRollDuration", &attributes.techRollDuration);
		ImGui::InputFloat("TechRollSpeed", &attributes.techRollSpeed);

		//render other attributes
		renderAttributes(window);
	}
	ImGui::End();
	ImGui::PopID();
}

void secro::PlayerCharacter::renderAttributes(sf::RenderWindow & window)
{
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

PhysicsHandle secro::PlayerCharacter::getPhysicsBody()
{
	return physicsBody;
}

BoxCollider & secro::PlayerCharacter::getPhysicsCollider()
{
	return level->getPhysics().getCollider(physicsBody);
}

bool secro::PlayerCharacter::isEqual(FacingDirection facing, Direction dir)
{
	return (facing == FacingDirection::Right && dir == Direction::Right) || (facing == FacingDirection::Left && dir == Direction::Left);
}

bool secro::PlayerCharacter::isOpposite(FacingDirection facing, Direction dir)
{
	return  (facing == FacingDirection::Right && dir == Direction::Left) || (facing == FacingDirection::Left && dir == Direction::Right);
}

float secro::PlayerCharacter::getWalkDeadzone()
{
	return walkDeadzone;
}

void secro::PlayerCharacter::knockBack(Vector2 knockback)
{
	if (isInvincible())
		return;

	if (knockback.y > 0.f && movementState == MovementState::OnGround)
	{
		knockback.y = -knockback.y;
		knockback = mul(knockback, 0.8f);
	}

	if (knockback.y < 0)
		movementState = MovementState::InAir;

	getPhysicsCollider().setVelocity(knockback);
}

void secro::PlayerCharacter::setMovementState(MovementState m)
{
	movementState = m;
}

void secro::PlayerCharacter::tryDoubleJump(float deltaTime)
{
	if (input->jumpPressed() && jumpsLeft > 0)
	{
		jumpsLeft--;
		Vector2 jumpVel = { 0.f, -attributes.doubleJumpSpeed };
		getPhysicsCollider().setVelocity(jumpVel);
		onDoubleJump();
		//stateMachine.changeState(this, PlayerState::Jump, 0.01666f);
	}
}

void secro::PlayerCharacter::updateHitstun(float deltaTime)
{
	if (hitstun >= 0.f)
		hitstun -= deltaTime;
}

bool secro::PlayerCharacter::isInHitstun()
{
	return hitstun > 0.f;
}

void secro::PlayerCharacter::putInHitstun(float duration)
{
	if (isInvincible())
		return;

	hitstun = duration;
	stateMachine.changeState(this, PlayerState::Hitstun, 0.016f);
	stateTimer = duration;
}

float secro::PlayerCharacter::getHitstun()
{
	return hitstun;
}

void secro::PlayerCharacter::updateState(float deltaTime)
{
	previousStateTimer = stateTimer;
	if (stateTimer >= 0.f)
		stateTimer -= deltaTime;

	stateMachine.update(deltaTime, this);
}

PlayerState secro::PlayerCharacter::getState()
{
	return state;
}

PlayerState secro::PlayerCharacter::getPreviousState()
{
	return previousState;
}

bool secro::PlayerCharacter::IsStateTimerDone()
{
	return stateTimer <= 0.f;
}

void secro::PlayerCharacter::setStateTimer(float seconds)
{
	stateTimer = seconds;
}

float secro::PlayerCharacter::getStateTimer()
{
	return stateTimer;
}

std::shared_ptr<Controller> secro::PlayerCharacter::getInput()
{
	return input;
}

float secro::PlayerCharacter::getDamage()
{
	return damage;
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
		if (currentAttackHitbox != HitboxId::None)
		{
			auto& currentMove = attackCollection.getAttack(state);
			auto& frames = currentMove.frames;
			float nextAttackTimer = attackTimer + deltaTime;

			//push new frames to hitbox if needed
			for (auto& it : frames)
			{
				if (it.time > attackTimer && it.time <= nextAttackTimer)
				{
					hitboxManager->getHitbox(currentAttackHitbox)->changeHitboxes(it.changes);
				}
			}

			//update the timer
			attackTimer = nextAttackTimer;
		}
		else
		{
			//std::cout << "ERROR, player is in attacking state, but does not have a current hitbox" << std::endl;
		}
	}
}

float secro::PlayerCharacter::getAttackTimer()
{
	return attackTimer;
}

void secro::PlayerCharacter::attackHasHit()
{
	if ((int)state > (int)PlayerState::AFirstAttack && (int)state < (int)PlayerState::ALastAttack)
	{
		hasAttackHit = true;
	}
}

bool secro::PlayerCharacter::getHasAttackHit()
{
	return hasAttackHit;
}

bool secro::PlayerCharacter::isAttacking()
{
	return (int)state > (int)PlayerState::AFirstAttack && (int)state < (int)PlayerState::ALastAttack;
}

void secro::PlayerCharacter::updateHitlag(float deltaTime)
{
	if (hitlag > 0.f)
	{
		hitlag -= deltaTime;

		if (hitlag <= 0.f)
		{
			getPhysicsCollider().setVelocity(hitlagVelocity);

			if (!isInvincible() && state == PlayerState::Hitstun)
				applyDI();
		}
	}
}

bool secro::PlayerCharacter::isInHitlag()
{
	return hitlag > 0.f;
}

void secro::PlayerCharacter::putInHitlag(float duration)
{
	auto& coll = getPhysicsCollider();
	hitlag = duration;
	hitlagVelocity = coll.getVelocity();
	coll.setVelocity(Vector2{ 0.f, 0.f });
	freezeShake = duration * freezeShakePerHitlag + freezeShakeBase;
}

b2Vec2 secro::PlayerCharacter::calcFreezeShake()
{
	if (isInHitlag() && getState() == PlayerState::Hitstun)
	{
		float r1 = 0.5f - static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float r2 = 0.5f - static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		b2Vec2 ret = { r1, r2 };
		return mul(ret, freezeShake * 2);
	}
	return { 0.f, 0.f };
}

void secro::PlayerCharacter::stateStartNewAttack(PlayerState attack)
{
	if (currentAttackHitbox != HitboxId::None)
	{
		hitboxManager->getHitbox(currentAttackHitbox)->destroy();
		currentAttackHitbox = HitboxId::None;
	}

	currentAttackState = attack;
	auto& att = attackCollection.getAttack(attack);
	currentAttackHitbox = hitboxManager->addHitbox(this, att);
	attackTimer = -0.01f;
	stateTimer = att.duration;
}

void secro::PlayerCharacter::stateStartShield()
{
	if (hurtboxFrames.frames.size() < 2)
	{
		//std::cout << "ERROR: player does not have a shielding state" << std::endl;
		return;
	}

	if (hurtbox != HitboxId::None)
		hitboxManager->getHurtbox(hurtbox)->changeHitboxes(hurtboxFrames.frames[1].changes);
}

void secro::PlayerCharacter::stateEndShield()
{
	if (hurtboxFrames.frames.size() < 1)
	{
		//std::cout << "ERROR: player does not have a hurtbox state" << std::endl;
		return;
	}

	if (hurtbox != HitboxId::None)
		hitboxManager->getHurtbox(hurtbox)->changeHitboxes(hurtboxFrames.frames[0].changes);
}

void secro::PlayerCharacter::stateUpdateDash(float deltaTime)
{
	auto dir = input->getMovementDirection();
	if (!isEqual(facingDirection, dir))
	{
		canSlideOffPlatforms = false;
	}
	else
	{
		canSlideOffPlatforms = true;

		//update dash speed if the player is holding the direction
		float timeScalar = (attributes.dashDuration - getStateTimer()) / attributes.dashDuration;
		auto& coll = getPhysicsCollider();
		auto vel = coll.getVelocity();
		if (facingDirection == FacingDirection::Left)
		{
			vel.x = -attributes.dashInitialSpeed * attributes.dashSpeedCurve.calculate(timeScalar);
		}
		else //right
		{
			vel.x = attributes.dashInitialSpeed * attributes.dashSpeedCurve.calculate(timeScalar);
		}
		coll.setVelocity(vel);
	}
}

void secro::PlayerCharacter::stateUpdateStand(float deltaTime)
{
	auto& coll = getPhysicsCollider();
	auto vel = coll.getVelocity();

	if (vel.x > 0.5f)
	{
		if (facingDirection == FacingDirection::Right)
			canSlideOffPlatforms = false;
		else
			canSlideOffPlatforms = true;
	}
	else if (vel.x < -0.5f)
	{
		if (facingDirection == FacingDirection::Left)
			canSlideOffPlatforms = false;
		else
			canSlideOffPlatforms = true;
	}
	else 
	{
		canSlideOffPlatforms = true;
	}
}

bool secro::PlayerCharacter::stateCanTech()
{
	//temp
	return previousMovementState == MovementState::InAir && movementState == MovementState::OnGround &&
		input->blockHeld();
}

bool secro::PlayerCharacter::stateCanTechEnd()
{
	return IsStateTimerDone();
}

void secro::PlayerCharacter::stateTechLeftBegin()
{
	Vector2 vel = { -attributes.techRollSpeed, 0.f };
	getPhysicsCollider().setVelocity(vel);
	setStateTimer(attributes.techRollDuration);
	makeInvincible(attributes.techRollInvDuration);

	//character can not slide off platform during tech
	canSlideOffPlatforms = false;

	//character should not have any friction on the ground
	shouldHaveFriction = false;
}

void secro::PlayerCharacter::stateTechRightBegin()
{
	Vector2 vel = { attributes.techRollSpeed, 0.f };
	getPhysicsCollider().setVelocity(vel);
	setStateTimer(attributes.techRollDuration);
	makeInvincible(attributes.techRollInvDuration);

	//character can not slide off platform during tech
	canSlideOffPlatforms = false;

	//character should not have any friction on the ground
	shouldHaveFriction = false;
}

void secro::PlayerCharacter::stateTechInPlaceBegin()
{
	Vector2 vel = { 0.f, 0.f };
	getPhysicsCollider().setVelocity(vel);
	setStateTimer(attributes.techInPlaceDuration);
	makeInvincible(attributes.techInPlaceInvDuration);

	//character can not slide off platform during tech
	canSlideOffPlatforms = false;

	//character should not have any friction on the ground
	shouldHaveFriction = false;
}

void secro::PlayerCharacter::stateTechEnd()
{
	canSlideOffPlatforms = true;
	shouldHaveFriction = true;
}

bool secro::PlayerCharacter::stateCanAirdodge()
{
	auto dir = input->getMovementDirection();
	return dir != Direction::Neutral && input->blockHeld(); //temp
}

bool secro::PlayerCharacter::stateCanEarlyAirdodge()
{
	return stateCanAirdodge();// && stateTimer < 0.017f;
}

void secro::PlayerCharacter::stateAirdodgeStart()
{
	useGravity = false;

	//set velocity
	auto dir = conv<Vector2>(input->getMovement());
	if (dir.y > -50.f && dir.y < 0.f && movementState == MovementState::OnGround)
		dir.y = 0.f;
	dir = normalise(dir);
	airdodgeDirectrion = dir;
	dir *= attributes.airdodgeSpeed;
	getPhysicsCollider().setVelocity(dir);

	//check for air
	if (dir.y < 0.f)
	{
		movementState = MovementState::InAir;
	}

	//set state duration
	setStateTimer(attributes.airdodgeDuration);
}

void secro::PlayerCharacter::stateAirdodgeEnd()
{
	auto& coll = getPhysicsCollider();
	useGravity = true;

	if (movementState == MovementState::InAir)
	{
		Vector2 vel = mul(coll.getVelocity(), 0.3f);
		coll.setVelocity(vel);
	}
	else if (movementState == MovementState::OnGround)
	{
		setStateTimer(attributes.airdodgeLandingLag);
	}
}

void secro::PlayerCharacter::stateUpdateAirdodge(float deltaTime)
{
	auto& coll = getPhysicsCollider();
	auto vel = coll.getVelocity();
	if (vel.y > -0.1f)
	{
		if (snapToGround(0.5f, false))
		{
			setMovementState(MovementState::OnGround);
		}
	}

	float alpha = (attributes.airdodgeDuration - stateTimer) / attributes.airdodgeDuration;
	alpha = clampOne(alpha);
	//resizeVelocity(attributes.airdodgeSpeedCurve.calculate(alpha) * attributes.airdodgeSpeed);
	auto newVel = airdodgeDirectrion;
	newVel = mul(newVel, attributes.airdodgeSpeedCurve.calculate(alpha) * attributes.airdodgeSpeed);
	coll.setVelocity(newVel);

	//give invincibility once needed
	//only happens once during the airdodge
	if (attributes.airdodgeDuration - previousStateTimer < attributes.airdodgeInvStart &&
		attributes.airdodgeDuration - stateTimer >= attributes.airdodgeInvStart)
	{
		makeInvincible(attributes.airdodgeInvDuration);
	}
}

void secro::PlayerCharacter::updateInvincibility(float deltaTime)
{
	if (invincibilityTimer >= 0.f)
		invincibilityTimer -= deltaTime;
}

void secro::PlayerCharacter::makeInvincible(float duration)
{
	invincibilityTimer = duration;
}

bool secro::PlayerCharacter::isInvincible()
{
	return invincibilityTimer > 0.f;
}

void secro::PlayerCharacter::updateDI(float deltaTime)
{
	DI = input->getMovement().x / 100.f;
}

void secro::PlayerCharacter::applyDI()
{
	auto& coll = getPhysicsCollider();
	auto vel = coll.getVelocity();
	
	if (vel.x == 0.f && vel.y == 0.f)
		return;

	float angle = angleFromDirection(vel);
	float DIDiff = getDI(angle);
	auto newVel = adjustAngle(vel, DIDiff);
	coll.setVelocity(newVel);
}

float secro::PlayerCharacter::getDI(float angle)
{
	if (angle <= 0.f)
	{
		if (angle >= -180.f)
		{
			//up
			return DI * GameplaySettings::DIInfluence;
		}
		else
		{
			//down
			return -DI * GameplaySettings::DIInfluence;
		}
	}
	else 
	{
		if (angle < 180.f)
		{
			//down
			return -DI * GameplaySettings::DIInfluence;
		}
		else
		{
			//up
			return DI * GameplaySettings::DIInfluence;
		}
	}
}

void secro::PlayerCharacter::updateAirDashTimer(float deltaTime)
{
	if (movementState == MovementState::OnGround)
		isAirDashUsed = false;

	airDashTimer += deltaTime;
	if (airDashTimer > attributes.airDashInputTimeFrame)
	{
		resetAirDashTimer();
	}

	canAirDash = false;
	auto d0 = input->getMovementDirection();
	auto d1 = Controller::getDirection(input->getInput(1).leftStick);
	if (d1 == Direction::Neutral && (d0 == Direction::Left || d0 == Direction::Right))
	{
		if (airDashPreviousDirection == d0)
		{
			resetAirDashTimer();
			airDashDirection = d0;
			canAirDash = true;
		}
		else
		{
			resetAirDashTimer();
			airDashPreviousDirection = d0;
		}
	}
}

void secro::PlayerCharacter::conditionAirDash()
{
	if (canAirDash && !isAirDashUsed)
	{
		//start the air dash
		Vector2 dir = { 1.f, -0.3f };
		if (airDashDirection == Direction::Left)
			dir.x = -1.f;

		getPhysicsCollider().setVelocity(mul(dir, attributes.airDashSpeed));

		//airdash can no longer be used
		isAirDashUsed = true;
	}
}

void secro::PlayerCharacter::resetAirDashTimer()
{
	canAirDash = false;
	airDashPreviousDirection = Direction::Neutral;
	airDashTimer = 0.f;
}

void secro::PlayerCharacter::onDoubleJump()
{
}
