#pragma once
#include "UISelectable.h"
#include <functional>

namespace secro {
	class UIButton : public UISelectable {
	public:
		UIButton();
		
		UIResult processInput(const Controller& controller, UIContext context) override;

	public:
		std::function<void()> onPressed;
	};
}