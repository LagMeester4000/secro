#include "Construct.h"
#include "Entity.h"
#include "Factory.h"
#include <string>

sen::Entity * sen::constructEntity(const char * type)
{
	auto& factory = Factory<Entity>::getInstance();
	return factory.get(factory.getType(type)).construct();
}

sen::Entity * sen::constructAndLoadEntity(const char * type, sen::SerializerLoad & serializer)
{
	auto& factory = Factory<Entity>::getInstance();
	auto& facObj = factory.get(factory.getType(type));
	auto* ret = facObj.construct();
	facObj.serializeLoad(ret, serializer);
	return ret;
}

sen::Level * sen::constructLevel(const char * type)
{
	auto& factory = Factory<Level>::getInstance();
	return factory.get(factory.getType(type)).construct();
}

sen::Level * sen::constructAndLoadLevel(const char * type, SerializerLoad & serializer)
{
	auto& factory = Factory<Level>::getInstance();
	auto& facObj = factory.get(factory.getType(type));
	auto* ret = facObj.construct();
	facObj.serializeLoad(ret, serializer);
	return ret;
}

sen::Component * sen::constructComponent(const char * type)
{
	auto& factory = Factory<Component>::getInstance();
	return factory.get(factory.getType(type)).construct();
}

sen::Component * sen::constructAndLoadComponent(const char * type, SerializerLoad & serializer)
{
	auto& factory = Factory<Component>::getInstance();
	auto& facObj = factory.get(factory.getType(type));
	auto* ret = facObj.construct();
	facObj.serializeLoad(ret, serializer);
	return ret;
}
