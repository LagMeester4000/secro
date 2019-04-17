#include "Component.h"
#include "Entity.h"

using namespace sen;

sen::Component::Component()
{
}

Entity * sen::Component::getOwner()
{
	return owner;
}

Level * sen::Component::getLevel()
{
	return owner->getLevel();
}

float sen::Component::getDeltaTime()
{
	return 0.0f;
}

void sen::Component::destroy()
{
	destroyed = true;
}

void sen::_setupComponent(Component * component, Entity * owner)
{
	component->owner = owner;
}
