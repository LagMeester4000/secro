#pragma once
#include "ComponentManager.h"
#include <string>

namespace sen {
	class Level;

	class Entity : public ComponentManager {
	public:
		Entity();

		//get the current level
		Level* getLevel();

		//get the deltatime value
		float getDeltaTime();

		//mark this entity as destroyed
		void destroy();

		//check if the entity is removed
		bool isMarkedAsDestroyed();

		//get the name of the entity
		std::string getName();

		//set the name of the entity
		void setName(std::string name);

		//get the filter path
		std::string getFilterPath();

		//setup values for the entity
		friend static void _initEntity(Entity* entity, Level* level, std::string name, std::string filterPath);

		template<typename T>
		void serialize() {}

	private:
		Level* level;
		bool destroyed;

		std::string name;
		std::string filterPath;
	};
}