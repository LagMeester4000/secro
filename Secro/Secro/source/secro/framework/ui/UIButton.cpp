#include "UIButton.h"
#include "secro/framework/input/Controller.h"

using namespace secro;

secro::UIButton::UIButton()
{
}

UIResult secro::UIButton::processInput(const Controller & controller, UIContext context)
{
	UISelectable::processInput(controller, context);

	if (controller.specialPressed())
	{
		if (onPressed)
			onPressed();
	}
	return UIResult();
}
