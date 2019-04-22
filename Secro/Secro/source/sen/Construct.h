#pragma once

namespace sen {
	class Entity;
	class Level;
	class Component;
	class SerializerLoad;

	//news a new entity of specified type
	Entity* constructEntity(const char* type);

	//news and serializes entity
	Entity* constructAndLoadEntity(const char* type, SerializerLoad& serializer);

	//news a new level of specified type
	Level* constructLevel(const char* type);

	//news and serializes level
	Level* constructAndLoadLevel(const char* type, SerializerLoad& serializer);

	//news a new component of specified type
	Component* constructComponent(const char* type);

	//news and serializes component
	Component* constructAndLoadComponent(const char* type, SerializerLoad& serializer);
}