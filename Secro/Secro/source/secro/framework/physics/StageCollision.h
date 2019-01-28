#pragma once
#include <Box2D/Common/b2Math.h>

namespace sf {
	class RenderWindow;
}

//box2d
class b2Body;
class b2Fixture;
class b2World;

namespace secro {
	class StageCollision {
	public:
		StageCollision(b2World& world, b2Vec2 position, b2Vec2 size);
		StageCollision(const StageCollision&) = delete;
		StageCollision(StageCollision&&);
		~StageCollision();

		void render(sf::RenderWindow& window);

	private:
		void setupBody(b2World& world, b2Vec2 position, b2Vec2 size);

	public:
		b2Body* body;
		b2Fixture* fixture;

		//debug
		b2Vec2 size;
	};
}