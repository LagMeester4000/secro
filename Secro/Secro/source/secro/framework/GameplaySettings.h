#pragma once

namespace secro {
	class GameplaySettings {
	public:
		//render a screen to adjust the values
		static void render();

		//the formula that calculates knockback
		static float calculateKnockback(float playerDamage, float moveBaseKB, float moveKBGrowth);

		//the formula that calculates hitstun
		static float calculateHitstun(float hitDamage, float playerDamage, float hitKnockback, float adjustment);

		//scaler that linearly influences the hitstun
		static float hitstunScalar;

		//value that decides how much DI influences the knockback
		//in degrees
		static float DIInfluence;
	};
}