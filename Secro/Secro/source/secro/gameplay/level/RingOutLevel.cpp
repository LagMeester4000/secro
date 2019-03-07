#include "RingOutLevel.h"
#include "secro/framework/player/PlayerCharacter.h"
#include "secro/framework/resource/ResourceManager.h"
#include <SFML/Graphics.hpp>

secro::RingOutLevel::RingOutLevel(std::shared_ptr<InputManager> input)
	: Level(input)
{
}

void secro::RingOutLevel::init()
{
	Level::init();

	//make level layout
	physicsManager.addStageCollider(b2Vec2(0.f, 5.f), b2Vec2(8.f, 3.f));
	physicsManager.addStageCollider(b2Vec2(0.f, -3.3f), b2Vec2(2.f, 0.3f), true);
	physicsManager.addStageCollider(b2Vec2(4.f, -0.6f), b2Vec2(2.f, 0.3f), true);
	physicsManager.addStageCollider(b2Vec2(-4.f, -0.6f), b2Vec2(2.f, 0.3f), true);
	
	//set player stocks
	for (size_t i = 0; i < players.size(); ++i)
	{
		playerStocks.push_back(stockAmount);
	}

	//load font
	uiFont = &*ResourceManager<sf::Font>::get("LemonMilk.otf");
}

void secro::RingOutLevel::update(float deltaTime)
{
	Level::update(deltaTime);

	//respawn mechanic
	for (size_t i = 0; i < players.size(); ++i)
	{
		auto& it = players[i];
		auto pos = it->getPosition();
		if (pos.x < boundsMin.x || pos.x > boundsMax.x ||
			pos.y < boundsMin.y || pos.y > boundsMax.y)
		{
			it->reset({ 0.f, 0.f });
			playerStocks[i]--;
		}
	}
}

void secro::RingOutLevel::render(sf::RenderWindow & window)
{
	Level::render(window);
	renderScores(window);
	Level::cameraRender(window);
}

bool secro::RingOutLevel::isGameOver()
{
	for (auto& it : playerStocks)
	{
		if (it <= 0)
		{
			return true;
		}
	}

	return false;
}

void secro::RingOutLevel::renderScores(sf::RenderWindow & window)
{
	//capture the view and make a new one
	auto resetView = window.getView();
	sf::View view(sf::FloatRect{ 0.f, 0.f, 1920.f, 1080.f });
	window.setView(view);
	
	for (size_t i = 0; i < players.size(); ++i)
	{
		auto& p = players[i];
		renderScore(window, { 640.f + 400.f * (float)i, 900.f }, &*p, playerStocks[i]);
	}
	
	//reset the view
	window.setView(resetView);
}

void secro::RingOutLevel::renderScore(sf::RenderWindow & window, sf::Vector2f position, PlayerCharacter * player, int stocks)
{
	float textSize = 115.f;
	float stockSize = 25.f;
	float padding = 20.f;
	float stockPadding = 10.f;

	sf::Text text;
	text.setFont(*uiFont);
	text.setString(std::to_string((int)player->getDamage()) + "%");
	text.setCharacterSize((unsigned int)textSize);
	text.setFillColor(sf::Color::White);
	text.setPosition(position);
	window.draw(text);

	for (int i = 0; i < stocks; ++i)
	{
		sf::CircleShape stock(stockSize);
		stock.setPosition(position + sf::Vector2f((stockSize * 2.f + stockPadding) * (float)i, textSize / 2.f + stockSize + padding + stockSize));
		stock.setFillColor(sf::Color::Red);
		window.draw(stock);
	}
}
