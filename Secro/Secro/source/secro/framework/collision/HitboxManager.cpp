#include "HitboxManager.h"
#include "secro/framework/player/PlayerCharacter.h"
#include "secro/framework/DebugOptions.h"
#include "secro/framework/level/Level.h"
#include <algorithm>

using namespace secro;

secro::HitboxManager::HitboxManager()
{
}

void secro::HitboxManager::update(float deltaTime, Level& level)
{
	//remove dirty hitboxes
	deleteMarked();

	//check hitboxes with hurtboxes
	for (auto& hurt : hurtboxes)
	{
		for (auto& hit : hitboxes)
		{
			//skip if the same player
			if (hit->getOwner() == hurt->getOwner())
				continue;

			auto results = hit->collide(*hurt);

			if (results.hasHit)
			{
				for (auto& it : results.hits)
				{
					Entity* otherEnt = hit->getOwner();
					Entity* hitEnt = hurt->getOwner();
					if (auto* asPlayer = dynamic_cast<PlayerCharacter*>(hitEnt))
					{
						//ignore if it is the same hitbox
						if (asPlayer->getLastHitId() == it->hitNumber + hit->getHitId())
							continue;

						//make sure the player doesnt get hit twice by the same attack
						asPlayer->getLastHitId() = it->hitNumber + hit->getHitId();

						//actual knockback
						//flip if player is on other side
						if (!results.hurts[0]->isShieldBox || (results.hurts[0]->isShieldBox && results.hits[0]->isGrabBox))
						{
							if ((otherEnt->getPosition().x > hitEnt->getPosition().x && otherEnt->getFacingDirection() == FacingDirection::Right) ||
								(otherEnt->getPosition().x < hitEnt->getPosition().x && otherEnt->getFacingDirection() == FacingDirection::Left))
								results.hits[0]->knockbackPlayer(asPlayer, flip(hit->getOwner()->getFacingDirection()));
							else
								results.hits[0]->knockbackPlayer(asPlayer, hit->getOwner()->getFacingDirection());
						}
						
						//hitlag
						float hitlag = 0.06f + 0.09f * (results.hits[0]->knockbackPowerGrowth / 15.f);
						asPlayer->putInHitlag(hitlag);
						if (auto* otherPlayer = dynamic_cast<PlayerCharacter*>(hit->getOwner()))
						{
							otherPlayer->putInHitlag(hitlag);

							//has hit
							otherPlayer->attackHasHit();

							//camera shake
							level.getCamera().shakeRadius = hitlag;
							level.getCamera().shakeRadiusChange = 0.5f;

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
		it->debugRender(window);
	}

	for (auto& it : hurtboxes)
	{
		it->debugRender(window);
	}
}

std::shared_ptr<HitboxCollection> secro::HitboxManager::addHitbox(Entity * owner, FrameData & framedata)
{
	auto ret = std::make_shared<HitboxCollection>(owner, framedata, true);
	hitboxes.push_back(ret);
	return ret;
}

std::shared_ptr<HitboxCollection> secro::HitboxManager::addHurtbox(Entity * owner, FrameData & framedata)
{
	auto ret = std::make_shared<HitboxCollection>(owner, framedata, false);
	hurtboxes.push_back(ret);
	return ret;
}

void secro::HitboxManager::deleteMarked()
{
	for (size_t i = 0; i < hitboxes.size(); ++i)
	{
		if (hitboxes[i]->shouldDestroy())
		{
			hitboxes.erase(hitboxes.begin() + i);
			i--;
		}
	}

	for (size_t i = 0; i < hurtboxes.size(); ++i)
	{
		if (hurtboxes[i]->shouldDestroy())
		{
			hurtboxes.erase(hurtboxes.begin() + i);
			i--;
		}
	}
}
