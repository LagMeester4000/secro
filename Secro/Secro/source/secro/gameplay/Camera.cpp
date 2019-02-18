#include "Camera.h"
#include <SFML/Graphics.hpp>
#include "secro/framework/player/PlayerCharacter.h"
#include "secro/framework/detail/PlainVectorMath.h"

secro::Camera::Camera()
{
	speed = 200.f;
	zoomSpeed = 2.f;
	zoom = 0.f;

	minPlayerDistance = 2.f;
	maxPlayerDistance = 10.f;

	minZoomIn = 0.62f;
	maxZoomIn = 1.f;

	//setup frustum
	frustumWidth = 5.f;
	frustumHeight = 5.f;
}

void secro::Camera::update(float deltaTime, std::vector<PlayerCharacter*> players)
{
	sf::Vector2f avg = getPlayerAveragePosition(players);
	float newZoom = getTargetZoom(players);

	auto mov = avg - position;
	position = position + mov * speed * deltaTime * deltaTime;
	zoom = zoom + (newZoom - zoom) * zoomSpeed * deltaTime;

	clamp();
}

void secro::Camera::render(sf::RenderWindow & window)
{
	auto view = window.getView();

	if (firstRun)
	{
		screenSize = view.getSize();
		firstRun = false;
	}

	view.setCenter(position);
	view.setSize(screenSize * zoom);
	window.setView(view);
}

sf::Vector2f secro::Camera::getPlayerAveragePosition(std::vector<PlayerCharacter*>& players)
{
	float divi = (float)players.size();
	sf::Vector2f total = { 0.f, 0.f };
	
	for (auto& it : players)
	{
		total += convNR<sf::Vector2f>(it->getPosition());
	}

	total /= divi;

	return total;
}

sf::Vector2f secro::Camera::getMaxWidthAndHeightDiff(std::vector<PlayerCharacter*>& players)
{
	auto pos1 = players[0]->getPosition();
	auto pos2 = players[1]->getPosition();
	return convNR<sf::Vector2f>(secro::min(pos2, pos1));
}

float secro::Camera::getTargetZoom(std::vector<PlayerCharacter*>& players)
{
	auto diff = getMaxWidthAndHeightDiff(players);
	float len = length(diff);
	
	if (len < minPlayerDistance)
	{
		len = minPlayerDistance;
	}
	else if (len > maxPlayerDistance)
	{
		len = maxPlayerDistance;
	}

	float scale = (len - minPlayerDistance) / (maxPlayerDistance - minPlayerDistance);
	float zoomDiff = maxZoomIn - minZoomIn;

	return scale * zoomDiff + minZoomIn;
}

void secro::Camera::clamp()
{
	if (position.x < -frustumWidth)
		position.x = -frustumWidth;
	else if (position.x > frustumWidth)
		position.x = frustumWidth;

	if (position.y < -frustumHeight)
		position.y = -frustumHeight;
	else if (position.y > frustumHeight)
		position.y = frustumHeight;
}
