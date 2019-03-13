#include "Hitbox.h"
#include "HitboxChange.h"
#include "../GameplaySettings.h"
#include "../player/PlayerCharacter.h"
#include "../detail/PlainVectorMath.h"

using namespace secro;

void secro::Hitbox::update(HitboxChange & change)
{
	position = change.position;
	isHitbox = change.isHitbox;
	isActive = change.isActive;
	hitNumber = change.hitNumber;
	relativePriority = change.relativePriority;
	absolutePriority = change.absolutePriority;

	damage = change.damage;
	knockbackAngle = change.knockbackAngle;
	knockbackPowerBase = change.knockbackPowerBase;
	knockbackPowerGrowth = change.knockbackPowerGrowth;
	hitstunAdjustment = change.hitstunAdjustment;

	isGrabBox = change.isGrabBox;
	isShieldBox = change.isShieldBox;
}

void secro::Hitbox::knockbackPlayer(PlayerCharacter * player, FacingDirection direction)
{
	float knockbackDist = GameplaySettings::calculateKnockback(player->getDamageScalar(), knockbackPowerBase, knockbackPowerGrowth);
	float hitstun = GameplaySettings::calculateHitstun(damage, player->getDamageScalar(), knockbackDist, hitstunAdjustment);
	b2Vec2 knockback = mul(directionFromAngle<b2Vec2>(knockbackAngle), knockbackDist);

	if (direction == FacingDirection::Left)
		knockback.x = -knockback.x;

	player->addDamage(damage);
	player->putInHitstun(hitstun);
	player->knockBack(knockback);
}

b2Vec2 secro::Hitbox::getKnockback(PlayerCharacter * player)
{
	float knockbackDist = GameplaySettings::calculateKnockback(player->getDamageScalar(), knockbackPowerBase, knockbackPowerGrowth);
	return mul(directionFromAngle<b2Vec2>(knockbackAngle), knockbackDist);
}
