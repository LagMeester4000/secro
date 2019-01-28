#include "CircleHitbox.h"
#include <SFML/Graphics.hpp>
#include "../detail/PlainVectorMath.h"
#include "HitboxChange.h"

using namespace secro;

void secro::CircleHitbox::update(HitboxChange & change)
{
	Hitbox::update(change);
	radius = change.width;
}

void secro::CircleHitbox::render(sf::RenderWindow& window, b2Vec2 position, b2Vec2 scale)
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

Box secro::CircleHitbox::getSpace()
{
	Box ret;
	ret.p1.x = position.x - radius;
	ret.p1.y = position.y - radius;
	ret.p2.x = position.x + radius;
	ret.p2.y = position.y + radius;
	return ret;
}

bool secro::CircleHitbox::collide(Hitbox & other, b2Vec2 otherPosition, b2Vec2 otherScale, b2Vec2 position, b2Vec2 scale)
{
	if (auto* cir = dynamic_cast<CircleHitbox*>(&other))
	{
		auto aPosOther = otherPosition + mul(other.position, otherScale);
		auto aPosMy = position + mul(this->position, scale);

		auto allowedDist = this->radius * scale.y + cir->radius * otherScale.y;
		return distance(aPosOther, aPosMy) <= allowedDist;
	}
	return false;
}
