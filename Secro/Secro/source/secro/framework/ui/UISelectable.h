#pragma once
#include "UIElement.h"

namespace secro {
	class UISelectable : public UIElement {
		friend class UIMenu;
	public:

	private: //links
		UISelectable* leftElement = nullptr;
		UISelectable* rightElement = nullptr;
		UISelectable* upElement = nullptr;
		UISelectable* downElement = nullptr;

		bool selected = false;
	};
}