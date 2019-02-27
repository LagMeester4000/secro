#pragma once
#include "UIElement.h"

namespace secro {
	class UISelectable : public UIElement {
		friend class UIMenu;
	public:
		bool isSelected();

	public: //links
		UISelectable* leftElement = nullptr;
		UISelectable* rightElement = nullptr;
		UISelectable* upElement = nullptr;
		UISelectable* downElement = nullptr;

	private:
		bool selected = false;
	};
}