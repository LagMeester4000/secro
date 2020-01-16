#pragma once
#include "PlayerGraphicsCharacter.h"

namespace secro {
	class CharacterPsycho : public PlayerGraphicsCharacter {
	public:
		CharacterPsycho();
		CharacterPsycho(Level* level, HitboxManager* hitboxManager, PhysicsHandle body, std::shared_ptr<Controller> controller);

		void init() override;
		void loadAnimations() override;
		void setupAttacks(AttackCollection& attacks) override;
		void setupStates(StateMachine & sm) override;

		void update(float deltaTime) override;

	public:
		bool ConditionUpSpecial(float deltaTime);
		void BeginUpSpecial(float deltaTime);

	private:
		// Temp val that is reset when hitting the ground
		bool CanNeutralSpecial = true;

		float UpSpecialSpeed = 30.f;
	};
}