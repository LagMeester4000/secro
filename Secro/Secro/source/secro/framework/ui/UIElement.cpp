#include "UIElement.h"

using namespace secro;

bool secro::UIElement::isVisable()
{
	return visable;
}

void secro::UIElement::update(float deltaTime)
{
	for (auto& it : children)
	{
		it->update(deltaTime);
	}
}

UIResult secro::UIElement::processInput(const Controller & controller, UIContext context)
{
	for (auto& it : children)
	{
		auto res = it->processInput(controller, context);
		if (res.change != UIResult::Null)
			return res;
	}
	return UIResult();
}

void secro::UIElement::render(UITransform & transform, sf::RenderWindow & window)
{
	if (!visable)
		return;

	for (auto& it : children)
	{
		auto trans = makeTransform(transform, it->transform);
		it->render(trans, window);
	}
}

const UITransform & secro::UIElement::getTransform()
{
	return transform;
}

bool secro::UIElement::isVisableWhenNotOnTop()
{
	return visableWhenNotOnTop;
}

void secro::UIElement::addChild(std::shared_ptr<UIElement> newElement)
{
	children.push_back(newElement);
}

std::shared_ptr<UIElement> secro::UIElement::getChild(size_t index)
{
	return children[index];
}
