#include "UIManager.h"
#include "UIElement.h"
#include <SFML/Graphics.hpp>

using namespace secro;

secro::UIManager::UIManager(Game * game)
	: game(game)
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

	//begin the view
	auto oldView = window.getView();
	window.setView(view);

	auto trans = UITransform();
	for (size_t i = 0; i < frames.size() - 1; ++i)
	{
		auto& it = frames[i];
		if (it->isVisableWhenNotOnTop())
			it->render(trans, window);
	}
	frames[frames.size() - 1]->render(trans, window);

	//reset to old view
	window.setView(oldView);
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

void secro::UIManager::setView(sf::View newView)
{
	view = newView;
}

Game * secro::UIManager::getGame()
{
	return game;
}
