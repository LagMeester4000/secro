#include "Hitbox.h"
#include "HitboxChange.h"
#include "secro/framework/GameplaySettings.h"
#include "secro/framework/player/PlayerCharacter.h"
#include "secro/framework/detail/PlainVectorMath.h"
#include "secro/netplay/SerializeFunctions.h"
#include <SFML/Graphics.hpp>

using namespace secro;

void secro::Hitbox::update(HitboxChange & change)
{
	position = change.position;
	radius = change.width;
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

	extraFreezeFrames = change.extraFreezeFrames;
}

void secro::Hitbox::render(sf::RenderWindow & window, Vector2 position, Vector2 scale)
{
	auto actualPos = position + mul(this->position, scale);

	sf::CircleShape hit(radius * scale.y);

	hit.setPosition({ actualPos.x - radius * scale.y, actualPos.y - radius * scale.y });

	if (isHitbox)
		hit.setFillColor(sf::Color(255, 0, 0, 126));
	else
		hit.setFillColor(sf::Color(0, 255, 0, 126));

	window.draw(hit);
}

Box secro::Hitbox::getSpace()
{
	Box ret;
	ret.p1.x = position.x - radius;
	ret.p1.y = position.y - radius;
	ret.p2.x = position.x + radius;
	ret.p2.y = position.y + radius;
	return ret;
}

bool secro::Hitbox::collide(Hitbox & other, Vector2 otherPosition, Vector2 otherScale, Vector2 position, Vector2 scale)
{
	auto aPosOther = otherPosition + mul(other.position, otherScale);
	auto aPosMy = position + mul(this->position, scale);

	auto allowedDist = this->radius * scale.y + other.radius * otherScale.y;
	return distance(aPosOther, aPosMy) <= allowedDist;
}

void secro::Hitbox::knockbackPlayer(PlayerCharacter * player, FacingDirection direction)
{
	float knockbackDist = GameplaySettings::calculateKnockback(player->getDamageScalar(), knockbackPowerBase, knockbackPowerGrowth);
	float hitstun = GameplaySettings::calculateHitstun(damage, player->getDamageScalar(), knockbackDist, hitstunAdjustment);
	Vector2 knockback = mul(directionFromAngle<Vector2>(knockbackAngle), knockbackDist);

	if (direction == FacingDirection::Left)
		knockback.x = -knockback.x;

	player->addDamage(damage);
	player->putInHitstun(hitstun);
	player->knockBack(knockback);
}

Vector2 secro::Hitbox::getKnockback(PlayerCharacter * player)
{
	float knockbackDist = GameplaySettings::calculateKnockback(player->getDamageScalar(), knockbackPowerBase, knockbackPowerGrowth);
	return mul(directionFromAngle<Vector2>(knockbackAngle), knockbackDist);
}

void secro::Hitbox::netSerSave(RawSerializeBuffer & buff)
{
	buff.save(position);
	buff.save(radius);
	buff.save(isHitbox);
	buff.save(isActive);
	buff.save(hitNumber);
	buff.save(relativePriority);
	buff.save(absolutePriority);
	buff.save(isShieldBox);
	buff.save(isGrabBox);
	buff.save(damage);
	buff.save(knockbackAngle);
	buff.save(knockbackPowerBase);
	buff.save(knockbackPowerGrowth);
	buff.save(hitstunAdjustment);
	buff.save(extraFreezeFrames);
}

void secro::Hitbox::netSerLoad(RawSerializeBuffer & buff)
{
	buff.load(position);
	buff.load(radius);
	buff.load(isHitbox);
	buff.load(isActive);
	buff.load(hitNumber);
	buff.load(relativePriority);
	buff.load(absolutePriority);
	buff.load(isShieldBox);
	buff.load(isGrabBox);
	buff.load(damage);
	buff.load(knockbackAngle);
	buff.load(knockbackPowerBase);
	buff.load(knockbackPowerGrowth);
	buff.load(hitstunAdjustment);
	buff.load(extraFreezeFrames);
}
