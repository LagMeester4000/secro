#include "CharacterOki.h"
#include "secro/framework/detail/PlainVectorMath.h"
#include "secro/framework/level/Level.h"
#include "secro/framework/input/Controller.h"
#include <Box2D/Box2D.h>
#include <functional>
#include <SFML/Graphics.hpp>
#include <tuple>

secro::CharacterOki::CharacterOki(Level* level, HitboxManager * hitboxManager, b2Body * body, std::shared_ptr<Controller> controller)
	: PlayerCharacter(level, hitboxManager, body, controller)
{
}

secro::CharacterOki::CharacterOki()
{
}

void secro::CharacterOki::init()
{
	PlayerCharacter::init();

	//specialDuration = 0.20f;
	//specialSpeed = 15.f;
	//specialRemainSpeed = 0.4f;
	//specialGroundFriction = -100.f;
	//specialHyperJumpPower = 3.f;
	//specialHyperJumpHeight = 14.f;
	//specialAmountOfAirDash = 3;
	//
	//normalFriction = attributes.groundDeceleration;


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
	loadAnimation("Dashette-LandingLag.png", 1, false, 0.05f, animLandingLag);
	loadAnimation("Dashette-TechLeft.png", 8, false, 0.03f, animTechLeft);
	loadAnimation("Dashette-TechRight.png", 8, false, 0.05f, animTechRight);

	//particles
	loadAnimation("poof.png", 43.f, 37.f, 30, false, 0.01f, particleDash);
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

void secro::CharacterOki::setupStates(StateMachine & sm)
{
	PlayerCharacter::setupStates(sm);

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
	//auto& spDir = specialDirection;

	//specials 
	auto condHook = [&](float f) {
		auto dir = getInput()->getMovementDirection();
		return getInput()->specialPressed() && dir != Direction::Neutral;
	};
	//sm.addCondition(,)


	//particles
	auto* lev = level;
	sm.addSetState(PlayerState::Dash, [=](float f)
	{
		auto pos = getPosition();
		auto& particle = lev->getParticleSystem().spawnParticle();
		particle.animation.setAnimation(particleDash);
		particle.animation.setOrigin({ 21.5f, 18.5f });
		if (getFacingDirection() == FacingDirection::Left)
		{
			particle.animation.setScale({ -0.05f, 0.05f });
			particle.animation.setPosition({ pos.x + 0.5f, pos.y + 0.2f });
		}
		else
		{
			particle.animation.setScale({ 0.05f, 0.05f });
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

void secro::CharacterOki::setupAttacks(AttackCollection & atts)
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

void secro::CharacterOki::update(float deltaTime)
{
	PlayerCharacter::update(deltaTime);

	if (!isInHitlag())
		animatedSprite.update(sf::seconds(deltaTime));


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
				part.opacity = 80.f;
				part.useAnimation = true;
				part.inFrontOfCharacter = false;
				int rScale = rand() % 2;
				if (rScale == 0)
					rScale = -1;
				part.animation.setScale({ 0.025f, 0.025f * (float)rScale });
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
				part.useAnimation = false;
				part.inFrontOfCharacter = false;
			}
		}
	}
}

void secro::CharacterOki::render(sf::RenderWindow & window)
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

void secro::CharacterOki::addFrames(int amount, Animation & animation)
{
	for (int i = 0; i < amount; ++i)
	{
		animation.addFrame(sf::IntRect(i * 64, 0, 64, 64));
	}
}

void secro::CharacterOki::loadAnimation(std::string fileName, int frames, bool loop, float speed, Animation & animation)
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

void secro::CharacterOki::addFrames(float width, float height, int amount, Animation & animation)
{
	for (int i = 0; i < amount; ++i)
	{
		animation.addFrame(sf::IntRect(i * width, 0, width, height));
	}
}

void secro::CharacterOki::loadAnimation(std::string fileName, float width, float height, int frames, bool loop, float speed, Animation & animation)
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
