#include "InputManager.h"

using namespace secro;

secro::InputManager::InputManager()
{
}

void secro::InputManager::init(int amountOfControllers)
{
	if (controllers.size() > 0)
		return;

	for (int i = 0; i < amountOfControllers; ++i)
	{
		controllers.push_back(
			Controller::createController(i, false)
		);
	}
}

void secro::InputManager::update()
{
	for (auto& it : controllers)
	{
		it->update();
	}
}

std::shared_ptr<Controller> secro::InputManager::getController(int index)
{
	return controllers[index];
}
