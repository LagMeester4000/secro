#include "PlayerCollision.h"
#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>
#include "Filters.h"

secro::PlayerCollision::PlayerCollision(b2World & world, b2Vec2 pos)
{
	setupPlayerBody(world, pos);
}

secro::PlayerCollision::PlayerCollision(PlayerCollision &&other)
{
	playerBody = other.playerBody;
	other.playerBody = nullptr;
}

secro::PlayerCollision::~PlayerCollision()
{
	if (playerBody)
		playerBody->GetWorld()->DestroyBody(playerBody);
}

void secro::PlayerCollision::render(sf::RenderWindow& window)
{
	sf::ConvexShape convex = sf::ConvexShape();
	
	convex.setPointCount(4);
	
	for (int i = 0; i < 4; ++i)
	{
		auto pos = playerBody->GetPosition();
		auto* shape = (b2PolygonShape*)playerBody->GetFixtureList()[0].GetShape();
		auto vertex = shape->GetVertex(i);
		convex.setPoint(i, { vertex.x + pos.x, vertex.y + pos.y });
	}

	convex.setFillColor(sf::Color(255, 0, 0));

	window.draw(convex);
}

void secro::PlayerCollision::setupPlayerBody(b2World & world, b2Vec2 pos)
{
	//some preset settings
	b2BodyDef def = b2BodyDef();
	def.fixedRotation = true;
	def.allowSleep = false;
	def.type = b2_dynamicBody;
	def.position = pos;

	//make body vertices
	b2Vec2 points[4];
	points[0] = { 0.f, 1.f };//down
	points[1] = { 0.6f, 0.f };//right
	points[2] = { 0.f, -1.f };//up
	points[3] = { -0.6f, 0.f };//left
	
	//make diamond shape from poins
	b2PolygonShape diamond = b2PolygonShape();
	diamond.Set(points, 4);

	//make the body
	playerBody = world.CreateBody(&def);
	playerFixture = playerBody->CreateFixture(&diamond, 1.f);

	//make the filter
	b2Filter f;
	f.groupIndex = GROUP_PLAYER;
	playerFixture->SetFilterData(f);
}
