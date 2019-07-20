#pragma once
#include "Collider.h"

namespace secro {
	class BoxCollider : public Collider {
	public:
		BoxCollider();

		bool collide(sfloat deltaTime, BoxCollider& other) const;
		Vector2 findNormal(sfloat deltaTime, BoxCollider& other);

		Vector2 getSize() const;
		void setSize(Vector2 vec);

	private:
		Vector2 size;
	};
}