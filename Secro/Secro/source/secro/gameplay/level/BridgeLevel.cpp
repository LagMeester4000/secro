#include "BridgeLevel.h"
#include "secro/framework/player/PlayerCharacter.h"
#include "secro/framework/resource/ResourceManager.h"
#include <SFML/Graphics.hpp>

secro::BridgeLevel::BridgeLevel(std::shared_ptr<InputManager> input)
	: Level(input)
{
}

void secro::BridgeLevel::init()
{
	Level::init();

	//setup stage
	physicsManager.addStageCollider(b2Vec2(0.f, 5.f), b2Vec2(bounds.x + 2.f, 3.f));

	//set player points
	for (size_t i = 0; i < players.size(); ++i)
	{
		playerPoints.push_back(0);
	}

	//load font
	uiFont = &*ResourceManager<sf::Font>::get("LemonMilk.otf");
}

void secro::BridgeLevel::update(float deltaTime)
{
	Level::update(deltaTime);
	updateScores();
}

void secro::BridgeLevel::render(sf::RenderWindow & window)
{
	Level::render(window);
	renderScores(window);

	//render the bounds
	{//left
		sf::RectangleShape rect(sf::Vector2f{ 2.f, 20.f });
		rect.setFillColor(sf::Color(0, 0, 255, 128));
		rect.setPosition(-bounds.x - 2.f, -12.f);
		window.draw(rect);
	}
	{//right
		sf::RectangleShape rect(sf::Vector2f{ 2.f, 20.f });
		rect.setFillColor(sf::Color(255, 0, 0, 128));
		rect.setPosition(bounds.x, -12.f);
		window.draw(rect);
	}

	cameraRender(window);
}

bool secro::BridgeLevel::isGameOver()
{
	for (auto& it : playerPoints)
	{
		if (it >= pointAmount)
		{
			return true;
		}
	}

	return false;
}

void secro::BridgeLevel::updateScores()
{
	{
		auto i = 0;
		auto &it = players[i];
		auto pos = it->getPosition();
		if (pos.x > bounds.x && it->getMovementState() == MovementState::OnGround)
		{
			playerPoints[i]++;
			resetPlayers();
		}
		if (pos.y > bounds.y)
		{
			playerPoints[1 - i]++;
			resetPlayers();
		}
	}

	{
		auto i = 1;
		auto &it = players[i];
		auto pos = it->getPosition();
		if (pos.x < -bounds.x && it->getMovementState() == MovementState::OnGround)
		{
			playerPoints[i]++;
			resetPlayers();
		}
		if (pos.y > bounds.y)
		{
			playerPoints[1 - i]++;
			resetPlayers();
		}
	}
}

void secro::BridgeLevel::resetPlayers()
{
	players[0]->setPosition({ -bounds.x + 1.5f, 0.f }, true);
	players[1]->setPosition({ bounds.x - 1.5f, 0.f }, true);
}

void secro::BridgeLevel::renderScores(sf::RenderWindow & window)
{
	//capture the view and make a new one
	auto resetView = window.getView();
	sf::View view(sf::FloatRect{ 0.f, 0.f, 1920.f, 1080.f });
	window.setView(view);

	for (size_t i = 0; i < players.size(); ++i)
	{
		auto& p = players[i];
		renderScore(window, { 640.f + 400.f * (float)i, 900.f }, &*p, playerPoints[i]);
	}

	//reset the view
	window.setView(resetView);
}

void secro::BridgeLevel::renderScore(sf::RenderWindow & window, sf::Vector2f position, PlayerCharacter * player, int stocks)
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
