#include "InputManager.h"
#include <SFML/Window.hpp>
#include <iostream>

using namespace secro;

secro::InputManager::InputManager()
{
}

void secro::InputManager::init(int amountOfControllers)
{
	if (controllers.size() > 0)
		return;

	int foundControllers = 0;
	for (int i = 0; i < 8; ++i)
	{
		if (sf::Joystick::isConnected((unsigned)i))
			foundControllers++;
		else
			continue;

		controllers.push_back(
			Controller::createController(controllers.size(), i, false)
		);

		std::cout << "Registered controller (" << foundControllers - 1 << ") with index " << i << std::endl;

		if (foundControllers == amountOfControllers)
			return;
	}

	//print error
	std::cout << "Could only find " << foundControllers << " controller(s)!" << std::endl;

	for (int i = foundControllers; i < amountOfControllers; ++i)
	{
		controllers.push_back(
			Controller::createController(8, false)
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

const std::vector<std::shared_ptr<Controller>>& secro::InputManager::getControllers()
{
	return controllers;
}
