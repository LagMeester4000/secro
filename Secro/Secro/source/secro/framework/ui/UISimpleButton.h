#pragma once
#include "UIButton.h"
#include <string>
#include <SFML/Graphics/Font.hpp>

namespace secro {
	class UISimpleButton : public UIButton {
	public:
		UISimpleButton();

		void render(UITransform& transform, sf::RenderWindow& window) override;

	public:
		sf::Vector2f size{ 60.f, 40.f };
		sf::Font font;
		float fontSize = 20.f;
		std::string text;
	};
}