#include "Collider.h"
#include "secro/framework/detail/PlainVectorMath.h"

using namespace secro;

secro::Collider::Collider()
{
}

void secro::Collider::update(sfloat deltaTime)
{
}

Vector2 secro::Collider::getPosition() const
{
	return position;
}

void secro::Collider::setPosition(Vector2 vec)
{
	position = vec;
}

void secro::Collider::updatePosition(sfloat deltaTime)
{
	position = plus(position, mul(velocity, deltaTime));
}

Vector2 secro::Collider::getVelocity() const
{
	return velocity;
}

void secro::Collider::setVelocity(Vector2 vec)
{
	velocity = vec;
}

ColliderProfile secro::Collider::getProfile() const
{
	return profile;
}

void secro::Collider::setProfile(ColliderProfile prof)
{
	profile = prof;
}
