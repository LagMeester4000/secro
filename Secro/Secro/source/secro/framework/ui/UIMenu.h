#pragma once
#include "UIElement.h"
#include <vector>
#include <memory>

namespace secro {
	class UISelectable;

	class UIMenu : public UIElement {
	public:
		UIMenu();

		void update(float deltaTime) override;
		UIResult processInput(const Controller& controller, UIContext context) override;
		void render(UITransform &transform, sf::RenderWindow& window) override;

		//add a selectable element
		void addSelectable(std::shared_ptr<UISelectable> selectable);

		//select one
		void setSelected(std::shared_ptr<UISelectable> selected);

	private:
		std::vector<std::shared_ptr<UISelectable>> items;
		UISelectable* selected;
	};
}