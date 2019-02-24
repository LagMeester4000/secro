#pragma once
#include "UIElement.h"
#include <vector>
#include <memory>

namespace secro {
	class UISelectable;

	class UIMenu : public UIElement {
	public:
		UIMenu();


	private:
		std::vector<std::shared_ptr<UISelectable>> items;
	};
}