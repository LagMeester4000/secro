#include "BoxCollider.h"
#include "secro/framework/detail/PlainVectorMath.h"

using namespace secro;

secro::BoxCollider::BoxCollider()
{
}

bool secro::BoxCollider::collide(sfloat deltaTime, BoxCollider & other) const
{
	auto posA = plus(getPosition(), mul(getVelocity(), deltaTime));
	auto posB = plus(other.getPosition(), mul(other.getVelocity(), deltaTime));

	auto sizeA = getSize();
	auto sizeB = other.getSize();

	sfloat half = 0.5f;
	if (posA.x - sizeA.x * half < posB.x + sizeB.x * half &&
		posA.x + sizeA.x * half > posB.x - sizeB.x * half &&
		posA.y - sizeA.y * half < posB.y + sizeB.y * half &&
		posA.y + sizeA.y * half > posB.y - sizeB.y * half)
	{
		return true;
	}
	return false;
}

secro::Vector2 secro::BoxCollider::findNormal(sfloat deltaTime, BoxCollider & other)
{
	auto posA = plus(getPosition(), mul(getVelocity(), deltaTime));
	auto posB = plus(other.getPosition(), mul(other.getVelocity(), deltaTime));

	auto sizeA = getSize();
	auto sizeB = other.getSize();

	sfloat half = 0.5f;
	sfloat xdis = 0.f;
	{
		//edge check
		sfloat dis0 = (posA.x + sizeA.x * half) - (posB.x - sizeB.x * half);
		if (dis0 >= 0.f)
			xdis = dis0;
		
		sfloat dis1 = (posB.x + sizeB.x * half) - (posA.x - sizeA.x * half);
		if (dis1 >= 0.f)
			xdis = dis1;
	}
	sfloat ydis = 0.f;
	{
		//edge check
		sfloat dis0 = (posA.y + sizeA.y * half) - (posB.y - sizeB.y * half);
		if (dis0 >= 0.f)
			ydis = dis0;

		sfloat dis1 = (posB.y + sizeB.y * half) - (posA.y - sizeA.y * half);
		if (dis1 >= 0.f)
			ydis = dis1;
	}

	return Vector2();
}

Vector2 secro::BoxCollider::getSize() const
{
	return size;
}

void secro::BoxCollider::setSize(Vector2 vec)
{
	size = vec;
}
