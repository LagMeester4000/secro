#pragma once

namespace sen {
	class Entity;
	class Level;
	class Component;
	class SerializerLoad;
	class SerializerSave;

	//serialize the given entity (as its child class)
	void saveEntity(Entity* entity, SerializerSave& serializer);

	//news a new entity of specified type
	Entity* constructEntity(const char* type);

	//news and serializes entity
	Entity* constructAndLoadEntity(const char* type, SerializerLoad& serializer);

	//serialize the given level (as its child class)
	void saveLevel(Level* level, SerializerSave& serializer);

	//news a new level of specified type
	Level* constructLevel(const char* type);

	//news and serializes level
	Level* constructAndLoadLevel(const char* type, SerializerLoad& serializer);

	//serialize the given component (as its child class)
	void saveComponent(Component* component, SerializerSave& serializer);

	//news a new component of specified type
	Component* constructComponent(const char* type);

	//news and serializes component
	Component* constructAndLoadComponent(const char* type, SerializerLoad& serializer);
}