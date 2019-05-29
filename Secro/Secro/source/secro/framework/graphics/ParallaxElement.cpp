#include "ParallaxElement.h"
#include "secro/gameplay/Camera.h"
#include <SFML/Graphics.hpp>

secro::ParallaxElement::ParallaxElement()
{
}

void secro::ParallaxElement::update(const Camera & cam)
{
	sf::Vector2f camPos = cam.getPosition();
	camPos *= -1.f;
	camPos.x *= cameraFactor.x;
	camPos.y *= cameraFactor.y;

	currentPosition = startPosition + camPos;
}

void secro::ParallaxElement::render(sf::RenderWindow & window)
{
	auto scale = sprite.getScale();
	auto rectI = sprite.getTextureRect();
	auto rect = sf::FloatRect(rectI);
	rect.width *= scale.x;
	rect.left *= scale.x;
	rect.height *= scale.y;
	rect.top *= scale.y;

	for (int x = 0; x < width; ++x)
	{
		for (int y = 0; y < height; ++y)
		{
			int ix = x - width / 2;
			int iy = y - height / 2;

			sprite.setPosition(currentPosition.x + rect.width * (float)ix, currentPosition.y + rect.height * (float)iy);
			window.draw(sprite);
		}
	}

	sprite.setPosition(0.f, 0.f);
}
