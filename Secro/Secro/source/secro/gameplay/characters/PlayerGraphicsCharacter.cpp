#include "PlayerGraphicsCharacter.h"
#include "secro/framework/detail/PlainVectorMath.h"
#include "secro/framework/level/Level.h"
#include <Box2D/Box2D.h>
#include <functional>
#include <SFML/Graphics.hpp>
#include <tuple>

secro::PlayerGraphicsCharacter::PlayerGraphicsCharacter(Level* level, HitboxManager * hitboxManager, b2Body * body, std::shared_ptr<Controller> controller)
	: PlayerCharacter(level, hitboxManager, body, controller)
{
}

secro::PlayerGraphicsCharacter::PlayerGraphicsCharacter()
{
}

void secro::PlayerGraphicsCharacter::init()
{
	PlayerCharacter::init();

	//particles
	loadAnimation("P_dash.png", 192, 192, 32.f, 32.f, 36, false, 0.0166666f, particleDash);
	loadAnimation("JumpDust.png", 7, false, 0.02f, particleJump);
	loadAnimation("hitstunPoof.png", 62.f, 45.f, 73, false, 0.0166f, particleHit);
	loadAnimation("FlyRing.png", 19.f, 45.f, 20, false, 0.0166f, particleFlyRing);
	loadAnimation("P_grab_effect.png", 96, 96, 32.f, 32.f, 9, false, 0.025f, particleGrab);

	//load child animations
	loadAnimations();

	animatedSprite.setAnimation(animRun);

	//load shield
	sf::Texture* shieldTexTemp = new sf::Texture();
	shieldTexTemp->loadFromFile("Shield.png");
	shieldSprite.setTexture(*shieldTexTemp);
	shieldSprite.setOrigin(32.f, 32.f);
}

void secro::PlayerGraphicsCharacter::loadAnimations()
{
}

void secro::PlayerGraphicsCharacter::setupStates(StateMachine & sm)
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
	sm.addSetState(PlayerState::AGrab, [=](float f)
	{
		auto pos = getPosition();
		auto& particle = lev->getParticleSystem().spawnParticle();
		particle.animation.setAnimation(particleGrab);
		particle.animation.setOrigin({ 16.f, 16.f });
		particle.animation.setScale({ 0.05f, 0.05f });
		particle.animation.setColor({ 255, 179, 128 });
		if (getFacingDirection() == FacingDirection::Right)
		{
			particle.animation.setPosition({ pos.x + 0.8f, pos.y + 0.2f });
		}
		else
		{
			particle.animation.setPosition({ pos.x - 0.8f, pos.y + 0.2f });
		}
	});
}

void secro::PlayerGraphicsCharacter::update(float deltaTime)
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

void secro::PlayerGraphicsCharacter::render(sf::RenderWindow & window)
{
	PlayerCharacter::render(window);
	//return;

	float scale = 1.f;
	if (facingDirection == FacingDirection::Left)
		scale = -1.f;

	auto pos = physicsBody->GetPosition();
	auto shake = calcFreezeShake();
	animatedSprite.setPosition(pos.x + shake.x, pos.y + 0.22f + shake.y);
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

void secro::PlayerGraphicsCharacter::addFrames(int amount, Animation & animation)
{
	for (int i = 0; i < amount; ++i)
	{
		animation.addFrame(sf::IntRect(i * 64, 0, 64, 64));
	}
}

void secro::PlayerGraphicsCharacter::loadAnimation(std::string fileName, int frames, bool loop, float speed, Animation & animation)
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

void secro::PlayerGraphicsCharacter::addFrames(float width, float height, int amount, Animation & animation)
{
	for (int i = 0; i < amount; ++i)
	{
		animation.addFrame(sf::IntRect(i * width, 0, width, height));
	}
}

void secro::PlayerGraphicsCharacter::loadAnimation(std::string fileName, float width, float height, int frames, bool loop, float speed, Animation & animation)
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

void secro::PlayerGraphicsCharacter::addFrames(int imgWidth, int imgHeight, float width, float height, int amount, Animation & animation)
{
	int framesWidth = imgWidth / width;
	for (int i = 0; i < amount; ++i)
	{
		animation.addFrame(sf::IntRect(i % framesWidth * width, i / framesWidth * height, width, height));
	}
}


void secro::PlayerGraphicsCharacter::loadAnimation(std::string fileName, int imgWidth, int imgHeight, float width, float height, int frames, bool loop, float speed, Animation & animation)
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