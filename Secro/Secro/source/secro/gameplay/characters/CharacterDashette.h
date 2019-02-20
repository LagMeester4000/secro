#pragma once
#include "secro/framework/player/PlayerCharacter.h"

namespace secro {
	class CharacterDashette : public PlayerCharacter {
	public:
		CharacterDashette(HitboxManager* hitboxManager, b2Body* body, std::shared_ptr<Controller> controller);

		void init() override;
		void setupStates(StateMachine& stateMachine) override;
		void setupAttacks(AttackCollection& attacks) override;
		void update(float deltaTime) override;
		void render(sf::RenderWindow& window) override;
		
	private: //special
		b2Vec2 specialDirection;
		float specialSpeed;
		float specialDuration;
		float specialRemainSpeed;
		float specialGroundFriction;
		float specialHyperJumpPower;
		float specialHyperJumpHeight;
		int specialAmountOfAirDash;

		float normalFriction;
		int airDashLeft;

	public:
		int getAirDashLeft();

	private:
		bool canBeginSpecial(float dt);
		bool canBeginSpecialFromAttack(float dt);
		void stateStartSpecial();
		void stateEndSpecial();
		void stateStartHyperJump();

	private: //graphics
	};
}