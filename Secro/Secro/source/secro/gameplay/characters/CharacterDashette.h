#pragma once
#include "PlayerGraphicsCharacter.h"
#include "secro/framework/graphics/AnimatedSprite.h"
#include "secro/framework/math/Curve.h"
#include <SFML/Graphics/Sprite.hpp>

namespace secro {
	class CharacterDashette : public PlayerGraphicsCharacter {
	public:
		CharacterDashette(Level* level, HitboxManager* hitboxManager, PhysicsHandle body, std::shared_ptr<Controller> controller);
		CharacterDashette();

		void init() override;
		void loadAnimations() override;
		void setupStates(StateMachine& stateMachine) override;
		void setupAttacks(AttackCollection& attacks) override;
		void update(float deltaTime) override;
		void render(sf::RenderWindow& window) override;
		void netSerSave(RawSerializeBuffer& buff) override;
		void netSerLoad(RawSerializeBuffer& buff) override;

	protected:
		void renderAttributes(sf::RenderWindow& window) override;

	private: //special
		Vector2 specialDirection;
		float specialSpeed;
		float specialDuration;
		float specialRemainSpeed;
		float specialGroundFriction;
		float specialHyperJumpPower;
		float specialHyperJumpHeight;
		int specialAmountOfAirDash;

		float normalFriction;
		int airDashLeft;

		Curve dashSpeedCurve;

	public:
		int getAirDashLeft();

	private:
		bool canBeginSpecial(float dt);
		bool canBeginSpecialFromAttack(float dt);
		void stateStartSpecial();
		void stateEndSpecial();
		void stateUpdateSpecial();
		void stateStartHyperJump();
		void stateStartShineSpecial();
		void stateUpdateShineSpecial(float deltaTime);

	private:
		Animation animSpDash;
		Animation animSpShine;
	};
}