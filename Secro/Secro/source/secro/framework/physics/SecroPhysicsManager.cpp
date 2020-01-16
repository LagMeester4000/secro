#include "SecroPhysicsManager.h"
#include "secro/framework/detail/PlainVectorMath.h"
#include "secro/framework/DebugOptions.h"

using namespace secro;

secro::SecroPhysicsManager::SecroPhysicsManager()
{
}

//void secro::SecroPhysicsManager::update(sfloat deltaTime)
//{
//	//values for subdivision of time
//	size_t div = 4;
//
//	//distance storage
//	struct DistTemp {
//		//bool done = false;
//		sfloat distance = 1.f;
//		Vector2 normal = Vector2{ 1.f, 1.f };
//	};
//	std::vector<DistTemp> dists(colliders.size());
//	
//	for (size_t i = 0; i < colliders.size(); ++i)
//	{
//		for (size_t j = 0; j < colliders.size(); ++j)
//		{
//			if (i == j /*|| dists[i].done*/)
//				continue;
//
//			sfloat sub = 1.f;
//			bool hit = false;
//			for (size_t x = 0; x < div; ++x)
//			{
//				if (!colliders[i].collide(sub * deltaTime, colliders[j]))
//					break;
//
//				hit = true;
//
//				if (x == div - 1)
//					sub /= 2.f;
//				else
//					sub = 0.f;
//			}
//
//			if (sub < dists[i].distance)
//				dists[i].distance = sub;
//
//			//fix normal
//			if (hit)
//			{
//				dists[i].normal = mul(dists[i].normal, colliders[i].findNormal(sub * deltaTime, colliders[j]));
//			}
//		}
//	}
//
//	//update all the positions and velocity
//	for (size_t i = 0; i < dists.size(); ++i)
//	{
//		colliders[i].updatePosition(dists[i].distance * deltaTime);
//		auto vel = colliders[i].getVelocity();
//		auto norm = dists[i].normal;
//		colliders[i].setVelocity(min(vel, mul(norm, dot(norm, vel))));
//	}
//}

//void secro::SecroPhysicsManager::update(sfloat deltaTime)
//{
//	//values for subdivision of time
//	size_t div = 4;
//
//	//distance storage
//	struct DistTemp {
//		//bool done = false;
//		sfloat distance = 1.f;
//		Vector2 normal = Vector2{ 1.f, 1.f };
//	};
//	std::vector<DistTemp> dists(colliders.size());
//
//	for (size_t i = 0; i < colliders.size(); ++i)
//	{
//		for (size_t j = 0; j < colliders.size(); ++j)
//		{
//			if (i == j /*|| dists[i].done*/)
//				continue;
//
//			//fix normal
//			if (colliders[i].collide(deltaTime, colliders[j]))
//			{
//				dists[i].normal = mul(dists[i].normal, colliders[i].findNormal(deltaTime, colliders[j]));
//			}
//		}
//	}
//
//	//update all the positions and velocity
//	for (size_t i = 0; i < dists.size(); ++i)
//	{
//		colliders[i].updatePosition(dists[i].distance * deltaTime);
//		auto vel = colliders[i].getVelocity();
//		auto norm = dists[i].normal;
//		colliders[i].setVelocity(min(vel, mul(norm, dot(norm, vel))));
//	}
//}

void secro::SecroPhysicsManager::update(sfloat deltaTime)
{
	size_t resultsSize = colliders.size();
	CollisionResult* results = new CollisionResult[resultsSize];

	{
		//horizontal checks
		memset((void*)results, 0, sizeof(CollisionResult) * resultsSize);
		for (size_t i = 0; i < colliders.size(); ++i)
		{
			auto& it = colliders[i];
			if (it.getProfile() == ColliderProfile::PlatformGround ||
				it.getProfile() == ColliderProfile::SolidGround)
				continue;

			results[i] = updateSide(i, deltaTime, false);
		}

		//update horizontal positions and velocities
		for (size_t i = 0; i < colliders.size(); ++i)
		{
			auto& it = results[i];
			auto& coll = colliders[i];
			if (it.hit)
			{
				auto vel = coll.getVelocity();
				vel.x = vel.x * it.timeOfHit;
				vel.y = 0.f;
				auto pos = coll.getPosition();
				coll.setPosition(pos + vel * deltaTime);
				auto newVel = coll.getVelocity();
				newVel.x = 0.f;
				coll.setVelocity(newVel);
			}
			else 
			{
				auto vel = coll.getVelocity();
				vel.y = 0.f;
				auto pos = coll.getPosition();
				coll.setPosition(pos + vel * deltaTime);
			}
		}
	}

	{
		//vertical checks
		memset((void*)results, 0, sizeof(CollisionResult) * resultsSize);
		for (size_t i = 0; i < colliders.size(); ++i)
		{
			auto& it = colliders[i];
			if (it.getProfile() == ColliderProfile::PlatformGround ||
				it.getProfile() == ColliderProfile::SolidGround)
				continue;

			results[i] = updateSide(i, deltaTime, true);
		}

		//update vertical positions and velocities
		for (size_t i = 0; i < colliders.size(); ++i)
		{
			auto& it = results[i];
			auto& coll = colliders[i];
			if (it.hit)
			{
				auto vel = coll.getVelocity();
				vel.y = vel.y * it.timeOfHit;
				vel.x = 0.f;
				auto pos = coll.getPosition();
				coll.setPosition(pos + vel * deltaTime);
				auto newVel = coll.getVelocity();
				newVel.y = 0.f;
				coll.setVelocity(newVel);
			}
			else
			{
				auto vel = coll.getVelocity();
				vel.x = 0.f;
				auto pos = coll.getPosition();
				coll.setPosition(pos + vel * deltaTime);
			}
		}
	}

	//dont forget to delete array
	delete[] results;
}

#include <SFML/Graphics.hpp>
void secro::SecroPhysicsManager::debugRender(sf::RenderWindow & window)
{
	if (!DebugOptions::getOptions().enablePhysicsRender)
		return;

	for (auto& it : colliders)
	{
		sf::RectangleShape r;
		r.setPosition(convNR<sf::Vector2f>(it.getPosition()));
		r.setSize(convNR<sf::Vector2f>(it.getSize()));
		r.setOrigin(convNR<sf::Vector2f>(it.getSize() / 2.f));
		
		auto prof = it.getProfile();
		if (prof == ColliderProfile::Entity ||
			prof == ColliderProfile::EntityNoPlatform)
		{
			r.setFillColor(sf::Color::Red);
		}
		else if (prof == ColliderProfile::SolidGround ||
			prof == ColliderProfile::PlatformGround)
		{
			r.setFillColor(sf::Color::Green);
		}

		window.draw(r);
	}
}

BoxCollider & secro::SecroPhysicsManager::getCollider(PhysicsHandle handle)
{
	return colliders[handle.handle];
}

PhysicsHandle secro::SecroPhysicsManager::addCollider(const BoxCollider & collider)
{
	PhysicsHandle ret;
	ret.handle = colliders.size();
	colliders.push_back(collider);
	return ret;
}

PhysicsHandle secro::SecroPhysicsManager::addPlayerCollider()
{
	BoxCollider coll;
	coll.setPosition({ -2.f, -5.f });
	coll.setSize(Vector2{ 1.f, 2.f });
	coll.setProfile(ColliderProfile::Entity);
	return addCollider(coll);
}

void secro::SecroPhysicsManager::rayCast(Vector2 point1, Vector2 point2, RayCastCallback * callback)
{
}

bool secro::SecroPhysicsManager::pushDown(PhysicsHandle handle, sfloat distance)
{
	if (handle.handle == -1)
		return false;

	auto& it = colliders[handle.handle];
	if (it.getProfile() == ColliderProfile::PlatformGround ||
		it.getProfile() == ColliderProfile::SolidGround)
		return false;

	auto testVel = Vector2{ 0.f, distance };
	auto oldVel = it.getVelocity();
	it.setVelocity(testVel);
	auto res = updateSide(handle.handle, 1.f, true);
	it.setVelocity(oldVel);

	if (res.hit)
	{
		//it.setPosition(it.getPosition() + testVel * res.timeOfHit);
		return true;
	}
	return false;
}

CollisionResult secro::SecroPhysicsManager::updateSide(size_t i, sfloat deltaTime, bool doVertical)
{
	static int subs = 3;
	CollisionResult ret;
	ret.hit = false;
	ret.timeOfHit = 1.f;

	auto& it = colliders[i];
	auto vel = it.getVelocity();// *deltaTime;
	auto pos = it.getPosition();

	if (doVertical)
		vel.x = 0.f;
	else
		vel.y = 0.f;

	for (size_t j = 0; j < colliders.size(); ++j)
	{
		if (i == j)
			continue;

		auto& jt = colliders[j];
		if (jt.getProfile() == ColliderProfile::Entity ||
			jt.getProfile() == ColliderProfile::EntityNoPlatform)
			continue;

		//check if entity should go through platform
		if (jt.getProfile() == ColliderProfile::PlatformGround)
		{
			if (it.getProfile() == ColliderProfile::EntityNoPlatform)
			{
				continue;
			}
			else if (it.getVelocity().y < 0.f)
			{
				continue;
			}
			else if (it.getPosition().y + it.getSize().y / 2.f >= jt.getPosition().y - jt.getSize().y / 2.f)
			{
				continue;
			}
		}

		//initial collision check
		CollisionResult tRet;
		if (it.collide(vel, deltaTime, jt))
		{
			tRet.hit = true;
			if (doVertical)
				tRet.normal = vel.y > 0.f ? Vector2{ 0.f, -1.f } : Vector2{ 0.f, 1.f };
			else
				tRet.normal = vel.x > 0.f ? Vector2{ -1.f, 0.f } : Vector2{ 1.f, 0.f };

			tRet.timeOfHit = 0.f;

			//try to find the point in time where the two shapes are no longer colliding
			// 1 to includig 3
			sfloat subIt = 0.5f;
			for (int s = 1; s <= subs; ++s)
			{
				sfloat sub = 1.f / std::powf(2, (sfloat)s + 1);

				if (it.collide(vel, subIt * deltaTime, jt))
				{
					//it's further away than subIt
					subIt -= sub;
				}
				else
				{
					//new furthest point found
					tRet.timeOfHit = subIt;

					//it's closer than subIt
					subIt += sub;
				}
			}

			//use the time point to calculate where the two shapes hit
			tRet.point = pos + vel * deltaTime * tRet.timeOfHit;
			tRet.point += Vector2{ -tRet.normal.x, -tRet.normal.y } * it.getSize();

			//try to replace final return
			if (tRet.timeOfHit <= ret.timeOfHit)
			{
				ret = tRet;
			}
		}
	}

	return ret;
}

void secro::SecroPhysicsManager::netSerSave(RawSerializeBuffer & buff)
{
	size_t size = colliders.size();
	buff.save(size);

	for (size_t i = 0; i < size; ++i)
	{
		colliders[i].netSerSave(buff);
	}
}

void secro::SecroPhysicsManager::netSerLoad(RawSerializeBuffer & buff)
{
	size_t size;
	buff.load(size);

	colliders.clear();
	for (size_t i = 0; i < size; ++i)
	{
		BoxCollider coll;
		coll.netSerLoad(buff);
		colliders.push_back(coll);
	}
}
