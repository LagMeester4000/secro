#include "HitboxManager.h"
#include "../player/PlayerCharacter.h"
#include <algorithm>

using namespace secro;

secro::HitboxManager::HitboxManager()
{
}

void secro::HitboxManager::update(float deltaTime)
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

			if (results.size() > 0)
			{
				std::sort(results.begin(), results.end(), [](auto& f1, auto& f2)
				{
					return f1->relativePriority > f2->relativePriority;
				});

				for (auto& it : results)
				{
					Entity* hitEnt = hurt->getOwner();
					if (auto* asPlayer = dynamic_cast<PlayerCharacter*>(hitEnt))
					{
						//ignore if it is the same hitbox
						if (asPlayer->getLastHitId() == it->hitNumber + hit->getHitId())
							continue;

						asPlayer->getLastHitId() = it->hitNumber + hit->getHitId();

						results[0]->knockbackPlayer(asPlayer, hit->getOwner()->getFacingDirection());
					}
				}

			}				
		}
	}
}

void secro::HitboxManager::render(sf::RenderWindow & window)
{
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
