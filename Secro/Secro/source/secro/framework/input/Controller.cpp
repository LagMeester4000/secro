#include "Controller.h"
#include <SFML/Window.hpp>
#include <iostream>
#include <math.h>
#include "../detail/PlainVectorMath.h"

using namespace secro;

const Joystick & secro::Controller::getDirAttack() const
{
	return current().rightStick;
}

Direction secro::Controller::getDirAttackDirection() const
{
	return getDirection(current().rightStick);
}

bool secro::Controller::attackPressed() const
{
	return current().bButton && !prev().bButton;
}

bool secro::Controller::attackHeld() const
{
	return current().bButton;
}

bool secro::Controller::attackReleased() const
{
	return !current().bButton && prev().bButton;
}

bool secro::Controller::specialPressed() const
{
	return current().aButton && !prev().aButton;
}

bool secro::Controller::specialHeld() const
{
	return current().aButton;
}

bool secro::Controller::specialReleased() const
{
	return !current().aButton && prev().aButton;
}

bool secro::Controller::blockPressed() const
{
	return current().rTrigger2 && !prev().rTrigger2;
}

bool secro::Controller::blockHeld() const
{
	return current().rTrigger2;
}

bool secro::Controller::blockReleased() const
{
	return !current().rTrigger2 && prev().rTrigger2;
}

bool secro::Controller::grabPressed() const
{
	return current().rTrigger1 && !prev().rTrigger1;
}

bool secro::Controller::grabHeld() const
{
	return current().rTrigger1;
}

bool secro::Controller::grabReleased() const
{
	return !current().rTrigger1 && prev().rTrigger1;
}

bool secro::Controller::jumpPressed() const
{
	return (current().yButton && !prev().yButton) || (current().xButton && !prev().xButton);
}

bool secro::Controller::jumpHeld() const
{
	return current().yButton || current().xButton;
}

bool secro::Controller::jumpReleased() const
{
	return !current().yButton && prev().yButton;
}

secro::Controller::Controller(int index, bool keyboard)
{
	controllerIndex = index;
	useKeyboard = keyboard;
}

void secro::Controller::swapBack()
{
	/*for (int i = 0; i < buffer - 1; ++i)
	{
		inputs[i] = inputs[i + 1];
	}*/

	for (int i = buffer - 1; i > 0; i--)
	{
		inputs[i] = inputs[i - 1];
	}
}

Direction secro::Controller::getDirection(const Joystick & stick) const
{
	if (stick.x > 70.7f)
	{
		return Direction::Right;
	}
	else if (stick.x < -70.7f)
	{
		return Direction::Left;
	}
	else if (stick.y > 70.7f)
	{
		return Direction::Down;
	}
	else if (stick.y < -70.7f)
	{
		return Direction::Up;
	}
	//else
	return Direction::Neutral;
}

const typename Controller::Input & secro::Controller::getInput(size_t index)
{
	return inputs[index];
}

secro::Controller::Input& secro::Controller::current()
{
	return inputs[0];
}

const secro::Controller::Input & secro::Controller::current() const
{
	return  inputs[0];
}

secro::Controller::Input & secro::Controller::prev(size_t index)
{
	return inputs[index];
}

const secro::Controller::Input & secro::Controller::prev(size_t index) const
{
	return inputs[index];
}

secro::Controller::Input & secro::Controller::prev()
{
	return inputs[1];
}

const secro::Controller::Input & secro::Controller::prev() const
{
	return inputs[1];
}

std::shared_ptr<Controller> secro::Controller::createController(int index, bool keyboard)
{
	class make_shared_enabler : public Controller {
	public:
		make_shared_enabler(int index, bool keyboard) : Controller(index, keyboard) {}
	};

	return std::make_shared<make_shared_enabler>(index, keyboard);
}

void secro::Controller::interceptController(std::function<void(Input&)> function)
{
	interceptFunction = function;
	useIntercept = true;
}

#include <iostream>
void secro::Controller::update()
{
	//swap old inputs away
	swapBack();

	//get new inputs
	if (!useIntercept)
	{
		if (sf::Joystick::isConnected(controllerIndex))
		{
			Input& i = current();

			i = readInput();

			////buttons
			//i.aButton = sf::Joystick::isButtonPressed(controllerIndex, 0);
			//i.bButton = sf::Joystick::isButtonPressed(controllerIndex, 1);
			//i.xButton = sf::Joystick::isButtonPressed(controllerIndex, 2);
			//i.yButton = sf::Joystick::isButtonPressed(controllerIndex, 3);
			//i.lTrigger1 = sf::Joystick::isButtonPressed(controllerIndex, 4);
			//i.rTrigger1 = sf::Joystick::isButtonPressed(controllerIndex, 5);
			//i.select = sf::Joystick::isButtonPressed(controllerIndex, 9);
			//i.start = sf::Joystick::isButtonPressed(controllerIndex, 8);
			//
			////triggers (need to be tested)
			//i.lTrigger2 = sf::Joystick::getAxisPosition(controllerIndex, sf::Joystick::Axis::Z) > 20.f;
			//i.rTrigger2 = sf::Joystick::getAxisPosition(controllerIndex, sf::Joystick::Axis::Z) < -20.f;
			//
			////joysticks
			//i.leftStick.x = sf::Joystick::getAxisPosition(controllerIndex, sf::Joystick::Axis::X);
			//i.leftStick.y = sf::Joystick::getAxisPosition(controllerIndex, sf::Joystick::Axis::Y);
			//
			//if (controllerIndex == 2)
			//{
			//	if (std::abs(i.leftStick.x) > 15.f)
			//	{
			//		int stop = 0;
			//	}
			//}
			//
			////joysticks
			//i.rightStick.x = sf::Joystick::getAxisPosition(controllerIndex, sf::Joystick::Axis::U);
			//i.rightStick.y = sf::Joystick::getAxisPosition(controllerIndex, sf::Joystick::Axis::V);
		}
		else
		{
			//std::cout << "controller not plugged in" << std::endl;
		}
	}
	else
	{
		interceptFunction(current());
	}
}

void secro::Controller::manualUpdate(Input & push)
{
	swapBack();
	current() = push;
}

secro::Controller::Input secro::Controller::readInput() const
{
	if (sf::Joystick::isConnected(controllerIndex))
	{
		Input i;

		//buttons
		i.aButton = sf::Joystick::isButtonPressed(controllerIndex, 0);
		i.bButton = sf::Joystick::isButtonPressed(controllerIndex, 1);
		i.xButton = sf::Joystick::isButtonPressed(controllerIndex, 2);
		i.yButton = sf::Joystick::isButtonPressed(controllerIndex, 3);
		i.lTrigger1 = sf::Joystick::isButtonPressed(controllerIndex, 4);
		i.rTrigger1 = sf::Joystick::isButtonPressed(controllerIndex, 5);
		i.select = sf::Joystick::isButtonPressed(controllerIndex, 9);
		i.start = sf::Joystick::isButtonPressed(controllerIndex, 8);

		//triggers (need to be tested)
		i.lTrigger2 = sf::Joystick::getAxisPosition(controllerIndex, sf::Joystick::Axis::Z) > 20.f;
		i.rTrigger2 = sf::Joystick::getAxisPosition(controllerIndex, sf::Joystick::Axis::Z) < -20.f;

		//joysticks
		i.leftStick.x = sf::Joystick::getAxisPosition(controllerIndex, sf::Joystick::Axis::X);
		i.leftStick.y = sf::Joystick::getAxisPosition(controllerIndex, sf::Joystick::Axis::Y);

		if (controllerIndex == 2)
		{
			if (std::abs(i.leftStick.x) > 15.f)
			{
				int stop = 0;
			}
		}

		//joysticks
		i.rightStick.x = sf::Joystick::getAxisPosition(controllerIndex, sf::Joystick::Axis::U);
		i.rightStick.y = sf::Joystick::getAxisPosition(controllerIndex, sf::Joystick::Axis::V);

		return i;
	}
	return Input();
}

const Joystick & secro::Controller::getMovement() const
{
	return current().leftStick;
}

Direction secro::Controller::getMovementDirection() const
{
	return getDirection(current().leftStick);
}

Direction secro::Controller::getMovementPushDirection() const
{
	auto prevDir = getDirection(prev(1).leftStick);
	auto curDir = getDirection(current().leftStick);

	if (prevDir == Direction::Neutral)
	{
		return curDir;
	}
	return Direction::Neutral;
}

DirectionPress secro::Controller::getMovementPushDirectionExt() const
{
	DirectionPress ret;
	ret.hardPush = false;

	auto& newJ = current().leftStick;
	auto& oldJ0 = prev().leftStick;
	auto& oldJ1 = prev(2).leftStick;

	ret.speed = (distance(newJ, oldJ0) + distance(oldJ0, oldJ1)) / 2.f;

	Direction newDir = getDirection(newJ);
	ret.direction = newDir;

	if (newDir == Direction::Right)
	{
		if (oldJ0.x < deadzone)
		{
			ret.hardPush = true;
		}
	}
	else if (newDir == Direction::Left)
	{
		if (oldJ0.x > -deadzone)
		{
			ret.hardPush = true;
		}
	}
	else if (newDir == Direction::Up)
	{
		if (oldJ0.y > -deadzone)
		{
			ret.hardPush = true;
		}
	}
	else if (newDir == Direction::Down)
	{
		if (oldJ0.y < deadzone)
		{
			ret.hardPush = true;
		}
	}

	return ret;
}
