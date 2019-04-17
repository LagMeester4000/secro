#include "CharacterDashette.h"
#include "secro/framework/detail/PlainVectorMath.h"
#include "secro/framework/level/Level.h"
#include <Box2D/Box2D.h>
#include <functional>
#include <SFML/Graphics.hpp>
#include <tuple>

secro::CharacterDashette::CharacterDashette(Level* level, HitboxManager * hitboxManager, b2Body * body, std::shared_ptr<Controller> controller)
	: PlayerCharacter(level, hitboxManager, body, controller)
{
}

secro::CharacterDashette::CharacterDashette()
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


	//load animations
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

	//particles
	loadAnimation("P_dash.png", 192, 192, 32.f, 32.f, 36, false, 0.0166666f, particleDash);
	loadAnimation("JumpDust.png", 7, false, 0.02f, particleJump);
	loadAnimation("hitstunPoof.png", 62.f, 45.f, 73, false, 0.0166f, particleHit);
	loadAnimation("FlyRing.png", 19.f, 45.f, 20, false, 0.0166f, particleFlyRing);

	animatedSprite.setAnimation(animRun);
	
	//load shield
	sf::Texture* shieldTexTemp = new sf::Texture();
	shieldTexTemp->loadFromFile("Shield.png");
	shieldSprite.setTexture(*shieldTexTemp);
	shieldSprite.setOrigin(32.f, 32.f);
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
	sm.addSetState(PlayerState::Jump, [&](float f)
	{
		animatedSprite.setAnimation(animInAir);
	});
	sm.addSetState(PlayerState::JumpSquat, [&](float f)
	{
		animatedSprite.setAnimation(animJumpSquat);
	});
	sm.addSetState(PlayerState::Dash, [&](float f)
	{
		animatedSprite.setAnimation(animDash);
	});
	sm.addSetState(PlayerState::Run, [&](float f)
	{
		animatedSprite.setAnimation(animRun);
	}); 
	sm.addSetState(PlayerState::Walk, [&](float f)
	{
		animatedSprite.setAnimation(animWalk);
	});
	sm.addSetState(PlayerState::Stand, [&](float f)
	{
		animatedSprite.setAnimation(animStand);
	});
	sm.addUnsetState(PlayerState::JumpSquat, [&](float f)
	{
		animatedSprite.setAnimation(animStand);
	});

	sm.addSetState(PlayerState::ANAir, [&](float f)
	{
		animatedSprite.setAnimation(animNAir);
	});
	sm.addSetState(PlayerState::AUAir, [&](float f)
	{
		animatedSprite.setAnimation(animUAir);
	});
	sm.addSetState(PlayerState::AFAir, [&](float f)
	{
		animatedSprite.setAnimation(animFAir);
	});
	sm.addSetState(PlayerState::ADAir, [&](float f)
	{
		animatedSprite.setAnimation(animDAir);
	});
	sm.addSetState(PlayerState::ABAir, [&](float f)
	{
		animatedSprite.setAnimation(animBAir);
	});
	sm.addSetState(PlayerState::AJab, [&](float f)
	{
		animatedSprite.setAnimation(animJab);
	});
	sm.addSetState(PlayerState::ADTilt, [&](float f)
	{
		animatedSprite.setAnimation(animDTilt);
	});
	sm.addSetState(PlayerState::AFTilt, [&](float f)
	{
		animatedSprite.setAnimation(animFTilt);
	});
	sm.addSetState(PlayerState::AUTilt, [&](float f)
	{
		animatedSprite.setAnimation(animUTilt);
	});
	sm.addSetState(PlayerState::AGrab, [&](float f)
	{
		animatedSprite.setAnimation(animGrab);
	}); 
	sm.addSetState(PlayerState::AGrabAir, [&](float f)
	{
		animatedSprite.setAnimation(animGrab);
	});
	sm.addSetState(PlayerState::Shield, [&](float f)
	{
		animatedSprite.setAnimation(animStand);
	});
	sm.addSetState(PlayerState::Hitstun, [&](float f)
	{
		animatedSprite.setAnimation(animHitstun);
	});
	sm.addSetState(PlayerState::SpecialFall, [&](float f)
	{
		animatedSprite.setAnimation(animFreeFall);
	});
	sm.addSetState(PlayerState::LandingLag, [&](float f)
	{
		animatedSprite.setAnimation(animLandingLag);
	}); 
	sm.addSetState(PlayerState::SpecialD, [&](float f)
	{
		animatedSprite.setAnimation(animSpShine);
	});
	sm.addSetState(PlayerState::TechRollLeft, [&](float f)
	{
		if (getFacingDirection() == FacingDirection::Right)
			animatedSprite.setAnimation(animTechLeft);
		else
			animatedSprite.setAnimation(animTechRight);
	});
	sm.addSetState(PlayerState::TechRollRight, [&](float f)
	{
		if (getFacingDirection() == FacingDirection::Right)
			animatedSprite.setAnimation(animTechRight);
		else
			animatedSprite.setAnimation(animTechLeft);
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


	//particles
	auto* lev = level;
	sm.addSetState(PlayerState::Dash, [=](float f) 
	{
		auto pos = getPosition();
		auto& particle = lev->getParticleSystem().spawnParticle();
		particle.animation.setAnimation(particleDash);
		particle.animation.setOrigin({ 16.f, 16.f });
		if (getFacingDirection() == FacingDirection::Left)
		{
			//particle.animation.setScale({ -0.05f, 0.05f });
			//particle.setScale({})
			particle.scale = { 0.05f, 0.05f };
			particle.animation.setPosition({ pos.x + 0.5f, pos.y + 0.2f });
		}
		else
		{
			//particle.animation.setScale({ 0.05f, 0.05f });
			particle.scale = { -0.05f, 0.05f };
			particle.animation.setPosition({ pos.x - 0.5f, pos.y + 0.2f });
		}
	});
	sm.addSetState(PlayerState::JumpSquat, [=](float f)
	{
		auto pos = getPosition();
		auto& particle = lev->getParticleSystem().spawnParticle();
		particle.animation.setAnimation(particleJump);
		particle.animation.setOrigin({ 32.f, 32.f });
		particle.animation.setScale({ 0.03f, 0.03f });
		particle.animation.setPosition({ pos.x, pos.y + 0.65f });
	});
	sm.addSetState(PlayerState::LandingLag, [=](float f)
	{
		{
			auto pos = getPosition();
			auto& particle = lev->getParticleSystem().spawnParticle();
			particle.animation.setAnimation(particleDash);
			particle.animation.setOrigin({ 10.5f, 7.5f });
			particle.animation.setScale({ 0.04f, 0.04f });
			particle.animation.setPosition({ pos.x - 0.5f, pos.y + 0.6f });
		}
		{
			auto pos = getPosition();
			auto& particle = lev->getParticleSystem().spawnParticle();
			particle.animation.setAnimation(particleDash);
			particle.animation.setOrigin({ 10.5f, 7.5f });
			particle.animation.setScale({ -0.04f, 0.04f });
			particle.animation.setPosition({ pos.x + 0.5f, pos.y + 0.6f });
		}
	});
	sm.addSetState(PlayerState::Hitstun, [&](float f) 
	{
		particleHitHitstunTimer = getHitstun() * particleHitHitstunScalar;
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
	PlayerCharacter::update(deltaTime);

	if (!isInHitlag())
		animatedSprite.update(sf::seconds(deltaTime));

	if (movementState == MovementState::OnGround)
	{
		airDashLeft = specialAmountOfAirDash;
	}


	//spawn hit particles
	if (getState() == PlayerState::Hitstun)
	{
		auto vel = getPhysicsBody()->GetLinearVelocity();
		auto speed = length(vel);
		std::cout << speed << std::endl;
		particleHitDuration = particleHitDurationMax / (speed / 10.f);
		if (particleHitDuration > particleHitDurationMax)
			particleHitDuration = particleHitDurationMax;

		particleHitHitstunTimer -= deltaTime;

		//if (particleHitHitstunTimer >= 0.f)
		{
			particleHitTimer -= deltaTime;
			particleFlyRingTimer -= deltaTime;

			if (particleHitTimer <= 0.f)
			{
				particleHitTimer = particleHitDuration;

				//spawn new particle
				auto pos = getPosition();
				auto& part = level->getParticleSystem().spawnParticle();
				part.animation.setAnimation(particleHit);
				part.animation.setOrigin({ 43.f, 22.5f });
				part.animation.setPosition(pos.x, pos.y);
				part.animation.setRotation(angleFromDirection(getPhysicsBody()->GetLinearVelocity()));
				part.opacityOverTime = -200.f;
				part.opacity = 100.f;
				//part.scale = { 0.2f, 0.2f };
				part.scaleOverTime = -0.1f;
				part.useAnimation = true;
				part.inFrontOfCharacter = false;
				int rScale = rand() % 2;
				if (rScale == 0)
					rScale = -1;
				part.scale = { 0.04f, 0.04f * (float)rScale };
			}

			if (particleFlyRingTimer <= 0.f)
			{
				particleFlyRingTimer = particleFlyRingDuration;

				//spawn new particle
				auto pos = getPosition();
				auto& part = level->getParticleSystem().spawnParticle();
				part.animation.setAnimation(particleFlyRing);
				part.animation.setOrigin({ 9.5f, 22.5f });
				part.animation.setScale({ 0.05f, 0.05f });
				part.animation.setPosition(pos.x, pos.y);
				part.animation.setRotation(angleFromDirection(getPhysicsBody()->GetLinearVelocity()));
				part.opacityOverTime = -200.f;
				part.opacity = 100.f;
				part.scale = { 0.05f, 0.05f };
				part.useAnimation = false;
				part.inFrontOfCharacter = false;
			}
		}
	}
}

void secro::CharacterDashette::render(sf::RenderWindow & window)
{
	//PlayerCharacter::render(window);
	//return;

	float scale = 1.f;
	if (facingDirection == FacingDirection::Left)
		scale = -1.f;

	auto pos = physicsBody->GetPosition();
	animatedSprite.setPosition(pos.x, pos.y + 0.22f);
	animatedSprite.setScale(sf::Vector2f(0.05f * scale, 0.05f));
	animatedSprite.setOrigin(32.f, 32.f);
	if (input->getPlayerIndex() == 1)
		animatedSprite.setColor(sf::Color(255, 122, 122));
	window.draw(animatedSprite);

	if (getState() == PlayerState::Shield)
	{
		shieldSprite.setPosition(pos.x, pos.y + 0.15f);
		shieldSprite.setScale(sf::Vector2f(0.05f * scale, 0.05f * scale));
		window.draw(shieldSprite);
	}
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

void secro::CharacterDashette::addFrames(int amount, Animation & animation)
{
	for (int i = 0; i < amount; ++i)
	{
		animation.addFrame(sf::IntRect(i * 64, 0, 64, 64));
	}
}

void secro::CharacterDashette::loadAnimation(std::string fileName, int frames, bool loop, float speed, Animation & animation)
{
	//TEMP
	sf::Texture* tex = new sf::Texture();
	if (!tex->loadFromFile(fileName))
	{
		std::cout << "could not load " << fileName << std::endl;
		return;
	}
	animation.setSpriteSheet(*tex);
	addFrames(frames, animation);
	animation.setLoops(loop);
	animation.setSpeed(speed);
}

void secro::CharacterDashette::addFrames(float width, float height, int amount, Animation & animation)
{
	for (int i = 0; i < amount; ++i)
	{
		animation.addFrame(sf::IntRect(i * width, 0, width, height));
	}
}

void secro::CharacterDashette::loadAnimation(std::string fileName, float width, float height, int frames, bool loop, float speed, Animation & animation)
{
	//TEMP
	sf::Texture* tex = new sf::Texture();
	if (!tex->loadFromFile(fileName))
	{
		std::cout << "could not load " << fileName << std::endl;
		return;
	}
	animation.setSpriteSheet(*tex);
	addFrames(width, height, frames, animation);
	animation.setLoops(loop);
	animation.setSpeed(speed);
}

void secro::CharacterDashette::addFrames(int imgWidth, int imgHeight, float width, float height, int amount, Animation & animation)
{
	int framesWidth = imgWidth / width;
	for (int i = 0; i < amount; ++i)
	{
		animation.addFrame(sf::IntRect(i % framesWidth * width, i / framesWidth * height, width, height));
	}
}


void secro::CharacterDashette::loadAnimation(std::string fileName, int imgWidth, int imgHeight, float width, float height, int frames, bool loop, float speed, Animation & animation)
{
	//TEMP
	sf::Texture* tex = new sf::Texture();
	if (!tex->loadFromFile(fileName))
	{
		std::cout << "could not load " << fileName << std::endl;
		return;
	}
	animation.setSpriteSheet(*tex);
	addFrames(imgWidth, imgHeight, width, height, frames, animation);
	animation.setLoops(loop);
	animation.setSpeed(speed);
}