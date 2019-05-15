#pragma once
#include "PlayerGraphicsCharacter.h"

namespace secro {
	class CharacterPsycho : public PlayerGraphicsCharacter {
	public:
		CharacterPsycho();
		CharacterPsycho(Level* level, HitboxManager* hitboxManager, b2Body* body, std::shared_ptr<Controller> controller);

		void init() override;
		void loadAnimations() override;
		void setupAttacks(AttackCollection& attacks) override;
	};
}