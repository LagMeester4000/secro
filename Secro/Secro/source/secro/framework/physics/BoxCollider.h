#pragma once
#include "Collider.h"
#include "RayCastResult.h"

namespace secro {
	class RawSerializeBuffer;

	class BoxCollider : public Collider {
	public:
		BoxCollider();

		bool collide(sfloat deltaTime, BoxCollider& other) const;
		bool collide(Vector2 vel, sfloat deltaTime, BoxCollider& other) const;
		Vector2 findNormal(sfloat deltaTime, BoxCollider& other);
		RayCastResult rayCast(Vector2 position1, Vector2 position2);

		Vector2 getSize() const;
		void setSize(Vector2 vec);

	public: //networking
		void netSerSave(RawSerializeBuffer& buff);
		void netSerLoad(RawSerializeBuffer& buff);

	private:
		Vector2 size;
	};
}