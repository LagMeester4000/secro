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
		float width = 6.f, height = 4.f;
		sf::Vector2f size{ 6.f, 4.f };
		sf::Font font;
		std::string text;
	};
}