#pragma once
#include "ComponentManager.h"
#include "MemberRef.h"
#include <vector>
#include <memory>

namespace sen {
	class Entity;

	class Level : public ComponentManager {
	public:
		Level();

		void update() override;

		//get the deltaTime value;
		float getDeltaTime();

		template<typename T>
		void serialize(T& t) 
		{
			t(
				MEMBER(entities),
				MEMBER(name),
				MEMBER(type)
			);

			ComponentManager::serialize(t);
		}

	private:
		//deltatime stored
		float deltaTime;

		//all the active entities in the level
		std::vector<std::shared_ptr<Entity>> entities;

		//name of the level
		std::string name;

		//type of the object
		std::string type;
	};
}