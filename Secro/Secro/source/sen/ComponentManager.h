#pragma once
#include "IUpdatable.h"

namespace sen {
	class ComponentManager : public IUpdatable {
	public:
		ComponentManager();

		template<typename T>
		void serialize(T& t)
		{
			//t()
		}
	};
}