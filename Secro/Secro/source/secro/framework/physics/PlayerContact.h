#pragma once
#include <Box2D/Box2D.h>

namespace secro {
	class PlayerContact : public b2ContactListener {
	public:
		void BeginContact(b2Contact* contact) override; 
		void EndContact(b2Contact* contact) override;
	};
}