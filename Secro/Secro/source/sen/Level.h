#pragma once
#include "ComponentManager.h"

namespace sen {
	class Level : public ComponentManager {
	public:
		Level();

		//get the deltaTime value;
		float getDeltaTime();

		template<typename T>
		void serialize() {}

	private:
		float deltaTime;
	};
}