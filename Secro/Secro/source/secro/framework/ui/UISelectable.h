#pragma once
#include "UIElement.h"

namespace secro {
	class UISelectable : public UIElement {
	public:

	private: //links
		UIElement* leftElement = nullptr;
		UIElement* rightElement = nullptr;
		UIElement* upElement = nullptr;
		UIElement* downElement = nullptr;
	};
}