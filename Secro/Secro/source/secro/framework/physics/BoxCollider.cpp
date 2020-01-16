#include "BoxCollider.h"
#include "secro/framework/detail/PlainVectorMath.h"
#include "secro/netplay/RawSerializeBuffer.h"
#include <math.h>

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

bool secro::BoxCollider::collide(Vector2 vel, sfloat deltaTime, BoxCollider & other) const
{
	auto posA = plus(getPosition(), vel * deltaTime);
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
			xdis = -dis1;
	}
	sfloat ydis = 0.f;
	{
		//edge check
		sfloat dis0 = (posA.y + sizeA.y * half) - (posB.y - sizeB.y * half);
		if (dis0 >= 0.f)
			ydis = dis0;

		sfloat dis1 = (posB.y + sizeB.y * half) - (posA.y - sizeA.y * half);
		if (dis1 >= 0.f)
			ydis = -dis1;
	}

	if (fabsf(ydis) > fabsf(xdis))
	{
		if (ydis > 0.f)
		{
			return Vector2{ 0.f, -1.f };
		}
		else 
		{
			return Vector2{ 0.f, 1.f };
		}
	}
	else 
	{
		if (xdis > 0.f)
		{
			return Vector2{ -1.f, 0.f };
		}
		else
		{
			return Vector2{ 1.f, 0.f };
		}
	}
}

RayCastResult secro::BoxCollider::rayCast(Vector2 position1, Vector2 position2)
{
	RayCastResult ret;

	//test top
	{
		Vector2 rel = min(position2, position1);
		
		if (rel.x == 0.f)
		{

		}
		else if (rel.y == 0.f)
		{
			//miss
		}
		else
		{
			sfloat relh = (position.y + size.y / 2.f) - position1.y;
			sfloat s = rel.y / rel.x;

			sfloat x = (-relh) / s;
			sfloat ax = x + position1.x;

			Vector2 point = Vector2{ ax, position.y + size.y };
			bool hasHit = ax >= position.x - size.x / 2.f && ax <= position.x + size.x / 2.f;
			if (hasHit)
			{
				//ret.alpha = 
				sfloat d1 = distance(point, position1);
				sfloat d2 = distance(point, position2);
				sfloat orD = distance(position1, position2);

				//confirm the alpha for the hit
				if (orD <= d1 && orD <= d2)
				{
					//hit confirmed!
					ret.alpha = d1 / orD;
					ret.normal = Vector2{ 0.f, 1.f };
					ret.hit = true;
				}
			}
		}
	}

	ret.hit = false;
	return ret;
}

Vector2 secro::BoxCollider::getSize() const
{
	return size;
}

void secro::BoxCollider::setSize(Vector2 vec)
{
	size = vec;
}

void secro::BoxCollider::netSerSave(RawSerializeBuffer & buff)
{
	buff.save(size);
	buff.save(position);
	buff.save(velocity);
	buff.save(profile);
}

void secro::BoxCollider::netSerLoad(RawSerializeBuffer & buff)
{
	buff.load(size);
	buff.load(position);
	buff.load(velocity);
	buff.load(profile);
}
