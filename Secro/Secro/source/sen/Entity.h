#pragma once
#include "ComponentManager.h"
#include "FilterPath.h"
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
		std::string getName() override;

		//set the name of the entity
		void setName(std::string name);

		//get the filter path ref
		FilterPath& getFilterPath();

		//get the type of the entity
		const std::string& getType();

		//setup values for the entity
		friend void _initEntity(Entity* entity, Level* level, std::string type, std::string name, std::string filterPath);
		friend void _initEntityPostSerialization(Entity* entity, Level* level);

		template<typename T>
		void serialize(T& t) {
			t(
				MEMBER(name), 
				MEMBER(filterPath),
				MEMBER(type)
			);

			ComponentManager::serialize(t);
		}

	private:
		Level* level;
		bool destroyed;

		std::string name;
		FilterPath filterPath;

		std::string type;
	};
}