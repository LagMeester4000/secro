#pragma once
#include <memory>
#include <Box2D/Box2D.h>

class b2Body;

namespace secro {
	class Controller;

	class PlayerMovement {
		enum class MovementState {
			OnGround,
			InAir
		};
	public:
		PlayerMovement(std::shared_ptr<Controller> controller, b2Body* playerBody);

		//update the movement
		void update(float deltaTime);

	private:
		void init();

	private:
		std::shared_ptr<Controller> input;
		b2Body* physicsBody;
		MovementState state;

		//callbacks
		struct RayToGround : b2RayCastCallback {
			float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point,
				const b2Vec2& normal, float32 fraction) override;
			PlayerMovement* mov;
		};
		friend struct RayToGround;
		RayToGround groundStickCallback;
	};
}