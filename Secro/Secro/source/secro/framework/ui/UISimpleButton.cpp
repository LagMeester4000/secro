#include "UISimpleButton.h"
#include "secro/framework/detail/PlainVectorMath.h"
#include <SFML/Graphics.hpp>

secro::UISimpleButton::UISimpleButton()
{
}

void secro::UISimpleButton::render(UITransform & transform, sf::RenderWindow & window)
{
	if (!visable)
		return;

	UIButton::render(transform, window);

	{
		sf::RectangleShape rectangle(mul(size, transform.scale));
		rectangle.setPosition(transform.position);
		if (isSelected())
			rectangle.setFillColor(sf::Color::Yellow);
		else
			rectangle.setFillColor(sf::Color(50, 50, 0));

		window.draw(rectangle);
	}

	sf::Text butText(text, font, 30.f * transform.scale.x);
	butText.setPosition(transform.position);
	sf::Vector2f textScale = { 0.01f, 0.01f };
	butText.setScale(mul(transform.scale, textScale));
	window.draw(butText);
}
