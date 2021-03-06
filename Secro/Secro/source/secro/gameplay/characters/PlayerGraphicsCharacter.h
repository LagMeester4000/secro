#pragma once
#include "secro/framework/player/PlayerCharacter.h"
#include "secro/framework/graphics/AnimatedSprite.h"
#include <SFML/Graphics/Sprite.hpp>

namespace secro {
	//aditional layer for graphics on top of the character
	class PlayerGraphicsCharacter : public PlayerCharacter {
	public:
		PlayerGraphicsCharacter(Level* level, HitboxManager* hitboxManager, PhysicsHandle body, std::shared_ptr<Controller> controller);
		PlayerGraphicsCharacter();

		void init() override;
		virtual void loadAnimations();
		void setupStates(StateMachine& stateMachine) override;
		void update(float deltaTime) override;
		void render(sf::RenderWindow& window) override;

		void onDoubleJump() override;

		void netSerSave(RawSerializeBuffer& buff) override;
		void netSerLoad(RawSerializeBuffer& buff) override;

	public: //graphics
		AnimatedSprite animatedSprite;
		sf::Sprite shieldSprite;

		Animation animStand;
		Animation animDash;
		Animation animRun;
		Animation animWalk;
		Animation animJumpSquat;
		Animation animInAir;
		Animation animInAirJump;
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
		Animation particleGrab;
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

		static void addFrames(int imgWidth, int imgHeight, float width, float height, int amount, Animation& animation);
		static void loadAnimation(std::string fileName, int imgWidth, int imgHeight, float width, float height, int frames, bool loop, float speed, Animation & animation);
	};
}