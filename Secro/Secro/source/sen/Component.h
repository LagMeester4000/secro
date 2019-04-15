#pragma once
#include "IUpdatable.h"

namespace sen {
	class Entity;
	class Level;

	class Component : public IUpdatable {
	public:
		Component();

		//get the owner entity
		Entity* getOwner();

		//get the current level (from the owner entity)
		Level* getLevel();

		//get the deltatime value
		float getDeltaTime();

		//mark this component as destroyed
		void destroy();

		//function for setting up the component for use
		friend static void _setupComponent(Component* component, Entity* owner);

		template<typename T>
		void serialize() {}

	private:
		Entity* owner;
		bool destroyed;
	};
}