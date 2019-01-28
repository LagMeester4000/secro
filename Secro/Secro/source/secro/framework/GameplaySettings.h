#pragma once

namespace secro {
	class GameplaySettings {
	public:
		//the formula that calculates knockback
		static float calculateKnockback(float playerDamage, float moveBaseKB, float moveKBGrowth);

		//the formula that calculates hitstun
		static float calculateHitstun(float hitDamage, float playerDamage, float hitKnockback, float adjustment);

		//scaler that linearly influences the hitstun
		static float hitstunScalar;
	};
}