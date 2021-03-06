#include "InputManager.h"
#include <SFML/Window.hpp>
#include <iostream>
#include "secro/netplay/RawSerializeBuffer.h"
#include "secro/framework/input/GamepadManager.h"

using namespace secro;

secro::InputManager::InputManager()
{
}

void secro::InputManager::init(int amountOfControllers)
{
	if (controllers.size() > 0)
		return;

	//int foundControllers = 0;
	//for (int i = 0; i < 8; ++i)
	//{
	//	if (sf::Joystick::isConnected((unsigned)i))
	//		foundControllers++;
	//	else
	//		continue;
	//
	//	controllers.push_back(
	//		Controller::createController(controllers.size(), i, false)
	//	);
	//
	//	std::cout << "Registered controller (" << foundControllers - 1 << ") with index " << i << std::endl;
	//
	//	if (foundControllers == amountOfControllers)
	//		return;
	//}
	//
	//
	//print error
	//std::cout << "Could only find " << foundControllers << " controller(s)!" << std::endl;
	//
	//for (int i = foundControllers; i < amountOfControllers; ++i)
	//{
	//	controllers.push_back(
	//		Controller::createController(-1, 0, false)
	//	);
	//}

	controllers.push_back(Controller::createController(controllers.size(), GamepadManager::createGamepad(), false));
	controllers.push_back(Controller::createController(controllers.size(), GamepadManager::createGamepad(), false));
	
}

void secro::InputManager::update()
{
	for (auto& it : controllers)
	{
		it->update();
	}
}

void secro::InputManager::updateNoInputs()
{
	for (auto& it : controllers)
	{
		it->emptyUpdate();
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

void secro::InputManager::netSerSave(RawSerializeBuffer & buff)
{
	for (auto it : controllers)
	{
		for (int i = 7; i >= 0; --i)
		{
			auto in = it->getInput(i);
			//auto comp = Controller::compressInput(in);
			buff.save(in);
		}
	}
}

void secro::InputManager::netSerLoad(RawSerializeBuffer & buff)
{
	for (auto it : controllers)
	{
		for (size_t i = 0; i < 8; ++i)
		{
			Controller::Input in;
			buff.load(in);
			it->manualUpdate(in);
		}
	}
}
