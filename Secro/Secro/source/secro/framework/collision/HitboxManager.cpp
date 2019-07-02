#include "HitboxManager.h"
#include "secro/framework/player/PlayerCharacter.h"
#include "secro/framework/DebugOptions.h"
#include "secro/framework/level/Level.h"
#include "secro/framework/GameplaySettings.h"
#include "secro/netplay/RawSerializeBuffer.h"
#include "secro/netplay/SerializeFunctions.h"
#include <algorithm>

using namespace secro;

secro::HitboxManager::HitboxManager()
{
}

void secro::HitboxManager::update(float deltaTime, Level& level)
{
	//check hitboxes with hurtboxes
	for (auto& hurt : hurtboxes)
	{
		for (auto& hit : hitboxes)
		{
			//skip if the same player
			//skip destroyed hit/hurtboxes
			if (hit.getOwner() == hurt.getOwner() || hit.shouldDestroy() || hurt.shouldDestroy())
				continue;

			auto results = hit.collide(hurt);

			if (results.hasHit)
			{
				for (auto& it : results.hits)
				{
					Entity* otherEnt = hit.getOwner();
					Entity* hitEnt = hurt.getOwner();

					//check if the hit is valid
					//ignore if it is the same hitbox
					if (hitEnt->getLastHitId() == it->hitNumber + hit.getHitId())
						continue;

					//make sure the player doesnt get hit twice by the same attack
					hitEnt->getLastHitId() = it->hitNumber + hit.getHitId();

					//call callback functions
					hitEnt->onReceiveHit(*results.hits[0], otherEnt);
					otherEnt->onSuccessfulHit(*results.hits[0], hitEnt);

					if (auto* asPlayer = dynamic_cast<PlayerCharacter*>(hitEnt))
					{
						//actual knockback
						//flip if player is on other side
						if (!results.hurts[0]->isShieldBox || (results.hurts[0]->isShieldBox && results.hits[0]->isGrabBox))
						{
							if ((otherEnt->getPosition().x > hitEnt->getPosition().x && otherEnt->getFacingDirection() == FacingDirection::Right) ||
								(otherEnt->getPosition().x < hitEnt->getPosition().x && otherEnt->getFacingDirection() == FacingDirection::Left))
								results.hits[0]->knockbackPlayer(asPlayer, flip(hit.getOwner()->getFacingDirection()));
							else
								results.hits[0]->knockbackPlayer(asPlayer, hit.getOwner()->getFacingDirection());
						}
						
						//hitlag
						float hitlag = 0.06f + 0.09f * (results.hits[0]->knockbackPowerGrowth / 15.f) + results.hits[0]->extraFreezeFrames;
						asPlayer->putInHitlag(hitlag);
						if (auto* otherPlayer = dynamic_cast<PlayerCharacter*>(hit.getOwner()))
						{
							otherPlayer->putInHitlag(hitlag);

							//has hit
							otherPlayer->attackHasHit();

							//camera shake
							auto& currentHit = results.hits[0];
							float shake = GameplaySettings::calculateKnockback(asPlayer->getDamageScalar(), currentHit->knockbackPowerBase, currentHit->knockbackPowerGrowth);
							level.getCamera().shakeRadius = shake / 100.f;
							level.getCamera().shakeRadiusChange = level.getCamera().shakeRadius / hitlag;

							//good
							//level.getCamera().shakeRadius = hitlag / 1.5f;
						}
					}
				}

			}
		}
	}
}

void secro::HitboxManager::render(sf::RenderWindow & window)
{
	if (!DebugOptions::getOptions().enableHitboxRender)
		return;

	for (auto& it : hitboxes)
	{
		if (it.shouldDestroy())
			continue;

		it.debugRender(window);
	}

	for (auto& it : hurtboxes)
	{
		if (it.shouldDestroy())
			continue;

		it.debugRender(window);
	}
}

HitboxId secro::HitboxManager::addHitbox(Entity * owner, FrameData & framedata)
{
	auto val = HitboxCollection(owner, framedata, true);

	for (size_t i = 0; i < hitboxes.size(); ++i)
	{
		auto& it = hitboxes[i];
		if (it.shouldDestroy())
		{
			it = val;
			HitboxId ret;
			ret.id = (int)i;
			return ret;
		}
	}

	hitboxes.push_back(val);
	HitboxId ret;
	ret.id = (int)hitboxes.size() - 1;
	return ret;
}

HitboxId secro::HitboxManager::addHurtbox(Entity * owner, FrameData & framedata)
{
	auto val = HitboxCollection(owner, framedata, false);

	for (size_t i = 0; i < hurtboxes.size(); ++i)
	{
		auto& it = hurtboxes[i];
		if (it.shouldDestroy())
		{
			it = val;
			HitboxId ret;
			ret.id = (int)i;
			return ret;
		}
	}

	hurtboxes.push_back(val);
	HitboxId ret;
	ret.id = (int)hurtboxes.size() - 1;
	return ret;
}

HitboxCollection* secro::HitboxManager::getHitbox(HitboxId id)
{
	if (id.id != HitboxId::None.id)
	{
		return &hitboxes[id.id];
	}
	return nullptr;
}

HitboxCollection* secro::HitboxManager::getHurtbox(HitboxId id)
{
	if (id.id != HitboxId::None.id)
	{
		return &hurtboxes[id.id];
	}
	return nullptr;
}

void secro::HitboxManager::netSerSave(RawSerializeBuffer & buff)
{
	size_t amountHitboxes = amountOfActiveHitboxes(hitboxes);
	buff.save(amountHitboxes);

	for (size_t i = 0; i < hitboxes.size(); ++i)
	{
		auto& it = hitboxes[i];
		if (!it.shouldDestroy())
		{
			buff.save(i);
			it.netSerSave(buff);
		}
	}

	size_t amountHurtboxes = amountOfActiveHitboxes(hurtboxes);
	buff.save(amountHurtboxes);

	for (size_t i = 0; i < hurtboxes.size(); ++i)
	{
		auto& it = hurtboxes[i];
		if (!it.shouldDestroy())
		{
			buff.save(i);
			it.netSerSave(buff);
		}
	}
}

void secro::HitboxManager::netSerLoad(RawSerializeBuffer & buff)
{
	//clear so that old ones can be rewritten in buffer while the newly added ones are removed
	clearHitboxes();
	
	//actual loading
	size_t amountHitboxes;
	buff.load(amountHitboxes);

	for (size_t i = 0; i < amountHitboxes; ++i)
	{
		size_t hitboxIndex;
		buff.load(hitboxIndex);
		HitboxCollection push;
		push.netSerLoad(buff);
		hitboxes[hitboxIndex] = push;
	}

	size_t amountHurtboxes;
	buff.load(amountHurtboxes);

	for (size_t i = 0; i < amountHurtboxes; ++i)
	{
		size_t hitboxIndex;
		buff.load(hitboxIndex);
		HitboxCollection push;
		push.netSerLoad(buff);
		hurtboxes[hitboxIndex] = push;
	}
}

void secro::HitboxManager::clearHitboxes()
{
	for (auto& it : hitboxes)
	{
		it.destroy();
	}

	for (auto& it : hurtboxes)
	{
		it.destroy();
	}
}

size_t secro::HitboxManager::amountOfActiveHitboxes(std::vector<HitboxCollection>& hitboxes)
{
	size_t ret = 0;

	for (auto& it : hitboxes)
	{
		if (!it.shouldDestroy())
			++ret;
	}

	return ret;
}
