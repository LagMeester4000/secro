#include "Entity.h"
#include "Level.h"

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
	return level->getDeltaTime();
}

void sen::Entity::destroy()
{
	destroyed = true;
}

bool sen::Entity::isMarkedAsDestroyed()
{
	return destroyed;
}

std::string sen::Entity::getName()
{
	return name;
}

void sen::Entity::setName(std::string name)
{
	this->name = name;
}

FilterPath & sen::Entity::getFilterPath()
{
	return filterPath;
}
