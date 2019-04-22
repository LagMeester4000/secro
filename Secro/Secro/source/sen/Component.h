#pragma once
#include "IUpdatable.h"
#include "MemberRef.h"

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

		virtual void onAttach() {}
		virtual void onDetach() {}

		//function for setting up the component for use
		friend static void _setupComponent(Component* component, Entity* owner, std::string type, std::string name);
		friend static void _setupComponentPostSerialization(Component* component, Entity* owner);

		template<typename T>
		void serialize(T& t) 
		{
			t(
				MEMBER(name),
				MEMBER(type)
			);
		}

	private:
		Entity* owner;
		bool destroyed;

		std::string name;

		std::string type;
	};
}