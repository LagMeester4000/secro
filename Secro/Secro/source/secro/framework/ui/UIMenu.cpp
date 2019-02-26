#include "UIMenu.h"
#include "UISelectable.h"
#include "secro/framework/input/Controller.h"

using namespace secro;

void secro::UIMenu::update(float deltaTime)
{
}

UIResult secro::UIMenu::processInput(const Controller & controller, UIContext context)
{
	if (selected)
		selected->processInput(controller, context);
	else
		return UIResult();

	auto dir = controller.getMovementPushDirection();
	if (dir == Direction::Down)
	{
		if (selected->downElement)
		{
			selected->selected = false;
			selected = selected->downElement;
			selected->selected = true;
		}
	}
	else if (dir == Direction::Up)
	{
		if (selected->upElement)
		{
			selected->selected = false;
			selected = selected->upElement;
			selected->selected = true;
		}
	}
	else if (dir == Direction::Left)
	{
		if (selected->leftElement)
		{
			selected->selected = false;
			selected = selected->leftElement;
			selected->selected = true;
		}
	}
	else if (dir == Direction::Right)
	{
		if (selected->rightElement)
		{
			selected->selected = false;
			selected = selected->rightElement;
			selected->selected = true;
		}
	}

	return UIResult();
}

void secro::UIMenu::render(UITransform & transform, sf::RenderWindow & window)
{
}

void secro::UIMenu::addSelectable(std::shared_ptr<UISelectable> selectable)
{
	if (selected)
	{
		selected->selected = false;
		selectable->selected = true;
		selected = &*selectable;
	}
}

void secro::UIMenu::setSelected(std::shared_ptr<UISelectable> selected)
{
}
