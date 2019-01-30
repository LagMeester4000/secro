#pragma once
#include <Box2D/Common/b2Math.h>

namespace sf {
	class RenderWindow;
}

//box2d
class b2Body;
class b2World;
class b2Fixture;

namespace secro {
	class PlayerCollision {
	public:
		PlayerCollision(b2World &world, b2Vec2 pos);
		PlayerCollision(const PlayerCollision&) = delete;
		PlayerCollision(PlayerCollision&&);
		~PlayerCollision();

		void render(class sf::RenderWindow& window);

	private:
		void setupPlayerBody(b2World& world, b2Vec2 pos);

	public:
		b2Body* playerBody;
		b2Fixture* playerFixture;
	};
}