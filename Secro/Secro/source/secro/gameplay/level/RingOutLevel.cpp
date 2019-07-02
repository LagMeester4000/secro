#include "RingOutLevel.h"
#include "secro/framework/player/PlayerCharacter.h"
#include "secro/framework/resource/ResourceManager.h"
#include "secro/netplay/SerializeFunctions.h"
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

	//load ground
	auto tex = ResourceManager<sf::Texture>::get("GroundTiles.png");
	tex->setSmooth(false);
	
	groundTiles.setTexture(*tex);
	groundTiles.setScale({ 0.05f, 0.05f });

	//load background
	auto groundTex = ResourceManager<sf::Texture>::get("content/levels/Background1.png");
	tex->setSmooth(false);

	background.sprite.setTexture(*groundTex);
	background.sprite.setScale({ 0.05f, 0.05f });
	background.width = 20;
	background.height = 20;
	background.cameraFactor.x = -0.4f;
	background.cameraFactor.y = -0.2f;
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

	//update background
	background.update(camera);
}

void secro::RingOutLevel::render(sf::RenderWindow & window)
{
	//render the background
	background.render(window);

	Level::render(window);

	//render the stage
	{
		sf::Vector2f start = { -8.f, 2.f };
		//top
		renderTile(window, start, { 1, 1 }, { 0, 0 });
		renderTile(window, tilePos(start, { 1, 0 }), { 18, 1 }, { 1, 0 });
		renderTile(window, tilePos(start, { 19, 0 }), { 1, 1 }, { 2, 0 });
		//mid
		renderTile(window, tilePos(start, { 0, 1 }), { 1, 7 }, { 0, 1 });
		renderTile(window, tilePos(start, { 19, 1 }), { 1, 7 }, { 2, 1 });
		renderTile(window, tilePos(start, { 1, 1 }), { 18, 7 }, { 1, 1 });
		//bottom
		renderTile(window, tilePos(start, { 0, 8 }), { 1, 1 }, { 0, 2 });
		renderTile(window, tilePos(start, { 1, 8 }), { 18, 1 }, { 1, 2 });
		renderTile(window, tilePos(start, { 19, 8 }), { 1, 1 }, { 2, 2 });
	}

	{
		sf::Vector2f start = { -2.f, -3.6f };
		//platforms
		renderTile(window, start, { 1, 1 }, { 0, 3 });
		renderTile(window, tilePos(start, { 1, 0 }), { 3, 1 }, { 1, 3 });
		renderTile(window, tilePos(start, { 4, 0 }), { 1, 1 }, { 2, 3 });
	}

	{
		sf::Vector2f start = { 2.f, -0.9f };
		//platforms
		renderTile(window, start, { 1, 1 }, { 0, 3 });
		renderTile(window, tilePos(start, { 1, 0 }), { 3, 1 }, { 1, 3 });
		renderTile(window, tilePos(start, { 4, 0 }), { 1, 1 }, { 2, 3 });
	}

	{
		sf::Vector2f start = { -6.f, -0.9f };
		//platforms
		renderTile(window, start, { 1, 1 }, { 0, 3 });
		renderTile(window, tilePos(start, { 1, 0 }), { 3, 1 }, { 1, 3 });
		renderTile(window, tilePos(start, { 4, 0 }), { 1, 1 }, { 2, 3 });
	}

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

void secro::RingOutLevel::netSerSave(RawSerializeBuffer & buff)
{
	Level::netSerSave(buff);

	size_t size = playerStocks.size();
	buff.save(size);

	for (auto& it : playerStocks)
	{
		buff.save(it);
	}
}

void secro::RingOutLevel::netSerLoad(RawSerializeBuffer & buff)
{
	Level::netSerLoad(buff);

	size_t size;
	buff.load(size);

	for (size_t i = 0; i < size; ++i)
	{
		buff.load(playerStocks[i]);
	}
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

sf::Vector2f secro::RingOutLevel::tilePos(sf::Vector2f begin, sf::Vector2i offset)
{
	return { begin.x + (float)offset.x * 0.05f * 16.f, begin.y + (float)offset.y * 0.05f * 16.f };
}

void secro::RingOutLevel::renderTile(sf::RenderWindow & window, sf::Vector2f position, sf::Vector2i size, sf::Vector2i tileIndex)
{
	groundTiles.setTextureRect(sf::IntRect{ tileIndex * 16, { 16, 16 } });

	for (int x = 0; x < size.x; ++x)
	{
		for (int y = 0; y < size.y; ++y)
		{
			groundTiles.setPosition(position.x + 0.05f * 16.f * (float)x, position.y + 0.05f * 16.f * (float)y);
			window.draw(groundTiles);
		}
	}
}
