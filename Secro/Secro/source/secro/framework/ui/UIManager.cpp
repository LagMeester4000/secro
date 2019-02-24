#include "UIManager.h"
#include "UIElement.h"

using namespace secro;

secro::UIManager::UIManager()
{
}

void secro::UIManager::update(float deltaTime)
{
	for (auto& it : frames)
	{
		it->update(deltaTime);
	}
}

void secro::UIManager::handleInput(const Controller & controller)
{
	auto top = getTopFrame();
	UIContext context;
	context.frame = &*top;
	context.manager = this;
	top->processInput(controller, context);
}

void secro::UIManager::render(sf::RenderWindow & window)
{
	if (frames.size() == 0)
		return;

	auto trans = UITransform();
	for (size_t i = 0; i < frames.size() - 1; ++i)
	{
		auto& it = frames[i];
		if (it->isVisableWhenNotOnTop())
			it->render(trans, window);
	}
	frames[0]->render(trans, window);
}

void secro::UIManager::pushTopFrame(std::shared_ptr<UIElement> frame)
{
	frames.push_back(frame);
}

void secro::UIManager::popTopFrame()
{
	frames.pop_back();
}

std::shared_ptr<UIElement> secro::UIManager::getTopFrame()
{
	return frames[frames.size() - 1];
}
