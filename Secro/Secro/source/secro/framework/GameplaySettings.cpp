#include "GameplaySettings.h"

float secro::GameplaySettings::hitstunScalar = 1.f;

float secro::GameplaySettings::calculateKnockback(float playerDamage, float moveBaseKB, float moveKBGrowth)
{
	return playerDamage * moveKBGrowth + moveBaseKB;
}

float secro::GameplaySettings::calculateHitstun(float hitDamage, float playerDamage, float hitKnockback, float adjustment)
{
	return hitKnockback * hitstunScalar + adjustment;
}
