#pragma once
#include "secro/framework/player/PlayerCharacter.h"
#include "secro/framework/graphics/AnimatedSprite.h"
#include <SFML/Graphics/Sprite.hpp>

namespace secro {
	class CharacterOki : public PlayerCharacter {
	public:
		CharacterOki(Level* level, HitboxManager* hitboxManager, PhysicsHandle body, std::shared_ptr<Controller> controller);
		CharacterOki();

		void init() override;
		void setupStates(StateMachine& stateMachine) override;
		void setupAttacks(AttackCollection& attacks) override;
		void update(float deltaTime) override;
		void render(sf::RenderWindow& window) override;

	private: //special
		//b2Vec2 specialDirection;
		//float specialSpeed;
		//float specialDuration;
		//float specialRemainSpeed;
		//float specialGroundFriction;
		//float specialHyperJumpPower;
		//float specialHyperJumpHeight;
		//int specialAmountOfAirDash;
		//
		//float normalFriction;
		//int airDashLeft;

	private: //new special
		//tweakable
		float hookRange;
		float hookDettachRange;
		float hookPullForce;

		//internal
		bool isHookOut;
		b2Vec2 hookPosition;
		float hookStartLength;


	public: //graphics
		AnimatedSprite animatedSprite;
		sf::Sprite shieldSprite;

		Animation animStand;
		Animation animDash;
		Animation animRun;
		Animation animWalk;
		Animation animJumpSquat;
		Animation animInAir;
		Animation animHitstun;
		Animation animFreeFall;
		Animation animLandingLag;
		Animation animTechLeft;
		Animation animTechRight;

		//attack
		Animation animNAir;
		Animation animUAir;
		Animation animFAir;
		Animation animDAir;
		Animation animBAir;
		Animation animJab;
		Animation animDTilt;
		Animation animFTilt;
		Animation animUTilt;
		Animation animGrab;

		//particles
		Animation particleDash;
		Animation particleJump;
		Animation particleHit;
		float particleHitDuration = 0.07f;
		float particleHitDurationMax = 0.07f;
		float particleHitTimer = 0.f;
		float particleHitHitstunScalar = 0.5f;
		float particleHitHitstunTimer = 0.f;
		Animation particleFlyRing;
		float particleFlyRingDuration = 0.27f;
		float particleFlyRingTimer = 0.f;

	public:
		static void addFrames(int amount, Animation& animation);
		static void loadAnimation(std::string fileName, int frames, bool loop, float speed, Animation& animation);

		static void addFrames(float width, float height, int amount, Animation& animation);
		static void loadAnimation(std::string fileName, float width, float height, int frames, bool loop, float speed, Animation& animation);
	};
}