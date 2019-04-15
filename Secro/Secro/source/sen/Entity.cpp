#include "Entity.h"

using namespace sen;

void sen::_initEntity(Entity * entity, Level * level, std::string name, std::string filterPath)
{
	entity->level = level;
	entity->name = name;
	entity->filterPath = filterPath;
}

sen::Entity::Entity()
{
}

Level * sen::Entity::getLevel()
{
	return level;
}

float sen::Entity::getDeltaTime()
{
	return 0.0f;
}

void sen::Entity::destroy()
{
	destroyed = true;
}

bool sen::Entity::isMarkedAsDestroyed()
{
	return destroyed;
}
