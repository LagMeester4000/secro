#include "StageCollision.h"
#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>
#include "Filters.h"

secro::StageCollision::StageCollision(b2World& world, b2Vec2 position, b2Vec2 size, bool isPlatform)
{
	setupBody(world, position, size, isPlatform);
}

secro::StageCollision::StageCollision(StageCollision &&other)
{
	size = other.size;

	body = other.body;
	fixture = other.fixture;
	other.body = nullptr;
	other.fixture = nullptr;
}

secro::StageCollision::~StageCollision()
{
	if (body)
		body->GetWorld()->DestroyBody(body);
}

void secro::StageCollision::render(sf::RenderWindow& window)
{
	sf::RectangleShape shape = sf::RectangleShape();

	auto position = body->GetPosition();
	
	shape.setSize({ size.x*2.f, size.y*2.f });
	shape.setPosition(position.x - size.x, position.y - size.y);

	shape.setFillColor(sf::Color(0, 255, 0));

	window.draw(shape);
}

void secro::StageCollision::setupBody(b2World& world, b2Vec2 position, b2Vec2 size, bool isPlatform)
{
	this->size = size;

	b2BodyDef bodyDef = b2BodyDef();
	bodyDef.position.Set(position.x, position.y);
	bodyDef.fixedRotation = true;

	b2PolygonShape box = b2PolygonShape();
	box.SetAsBox(size.x, size.y);

	body = world.CreateBody(&bodyDef);
	fixture = body->CreateFixture(&box, 0.f);

	if (isPlatform)
	{
		b2Filter f;
		f.categoryBits = CATEGORY_STAGE_PLATFORM;
		f.groupIndex = GROUP_PLAYER;
		fixture->SetFilterData(f);
	}
}
