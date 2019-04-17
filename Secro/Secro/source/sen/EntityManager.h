#pragma once
#include "IUpdatable.h"

namespace sen {
	class Level;

	class EntityManager final : public IUpdatable {
	public:
		EntityManager();

		void update() override;

		void inspect() override;



	private:
		Level* level;
	};
}