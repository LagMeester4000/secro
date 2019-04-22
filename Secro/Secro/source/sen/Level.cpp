#include "Level.h"
#include "Entity.h"

sen::Level::Level()
{
}

void sen::Level::update()
{
	//update all the entities
	for (auto& it : entities)
	{
		it->update();
	}

	//could be replaced with notify system (entity notifies level when destroyed)
	for (size_t i = 0; i < entities.size(); ++i)
	{
		auto& it = entities[i];
		if (it->isMarkedAsDestroyed())
		{
			entities.erase(entities.begin() + i);

			//repeat this index
			--i;
		}
	}

	ComponentManager::update();
}

float sen::Level::getDeltaTime()
{
	return deltaTime;
}
