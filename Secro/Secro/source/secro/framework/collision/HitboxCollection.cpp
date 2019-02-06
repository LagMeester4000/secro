#include "HitboxCollection.h"
#include "../Entity.h"
#include "FrameData.h"
#include "CircleHitbox.h"
#include <set>
#include <iostream>
#include <algorithm>

using namespace secro;

int HitboxCollection::hitIdCounter = 0;

secro::HitboxCollection::HitboxCollection(Entity * owner, FrameData & framedata, bool isHitbox)
{
	updatedFlag = false;
	relativeBox = { };
	this->owner = owner;
	hitboxes = std::vector<std::shared_ptr<Hitbox>>(framedata.amountOfHitboxes, nullptr);
	for (auto& it : hitboxes)
	{
		it = std::make_shared<CircleHitbox>();
	}

	//load hurtbox is this is a hurtbox
	if (!isHitbox)
	{
		changeHitboxes(framedata.frames[0].changes);
	}

	//make a new hitId
	hitId = makeHitId();
}

void secro::HitboxCollection::update()
{
	//not yet

	
}

HitResult secro::HitboxCollection::collide(HitboxCollection & other)
{
	//set can only have one of the same
	std::set<std::shared_ptr<Hitbox>> setHit;
	std::set<std::shared_ptr<Hitbox>> setHurt;

	//find scale and stuff
	auto otherPos = other.owner->getPosition();
	auto myPos = owner->getPosition();
	auto otherScale = scale(other.owner);
	auto myScale = scale(owner);

	for (auto& otherHit : other.hitboxes)
	{
		for (auto& myHit : hitboxes)
		{
			if (!myHit->isActive || !otherHit->isActive)
				continue;

			if (myHit->collide(
				*otherHit,
				otherPos,
				otherScale,
				myPos,
				myScale
			))
			{
				setHit.insert(myHit);
				setHurt.insert(otherHit);
			}
		}
	}

	//make return... LOL
	HitResult ret;
	ret.hits.reserve(setHit.size());
	for (auto& it : setHit)
	{
		ret.hits.push_back(it);
	}
	ret.hurts.reserve(setHit.size());
	for (auto& it : setHurt)
	{
		ret.hurts.push_back(it);
	}
	if (ret.hits.size() > 0)
	{
		ret.hasHit = true;
	}
	//sort results
	std::sort(ret.hits.begin(), ret.hits.end(), [](auto& f1, auto& f2) 
	{
		return f1->relativePriority > f2->relativePriority;
	});
	std::sort(ret.hurts.begin(), ret.hurts.end(), [](auto& f1, auto& f2)
	{
		return f1->relativePriority > f2->relativePriority;
	});

	return ret;
}

void secro::HitboxCollection::debugRender(sf::RenderWindow & window)
{
	for (auto& it : hitboxes)
	{
		if (it->isActive)
			it->render(window, owner->getPosition(), scale(owner));
	}
}

void secro::HitboxCollection::changeHitboxes(std::vector<HitboxChange>& changes)
{
	for (auto& it : changes)
	{
		if (it.index >= hitboxes.size())
		{
			std::cout << "nonexistant hitbox referenced" << std::endl;
			return;
		}

		changeHitboxes(it);
	}
}

void secro::HitboxCollection::changeHitboxes(HitboxChange & change)
{
	hitboxes[change.index]->update(change);
}

Box secro::HitboxCollection::getVolume()
{
	return Box();
}

void secro::HitboxCollection::destroy()
{
	shouldDelete = true;
}

bool secro::HitboxCollection::shouldDestroy()
{
	return shouldDelete;
}

Entity * secro::HitboxCollection::getOwner()
{
	return owner;
}

int secro::HitboxCollection::getHitId()
{
	return hitId;
}

void secro::HitboxCollection::updateRelativeBox()
{
}

b2Vec2 secro::HitboxCollection::scale(Entity * entity)
{
	auto dir = entity->getFacingDirection();

	if (dir == FacingDirection::Left)
	{
		return { -1.f, 1.f };
	}
	return { 1.f, 1.f };
}

int secro::HitboxCollection::makeHitId()
{
	return hitIdCounter++;
}
