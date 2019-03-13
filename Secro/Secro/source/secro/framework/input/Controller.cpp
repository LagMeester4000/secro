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
	return current().attackButton && !prev().attackButton;
}

bool secro::Controller::attackHeld() const
{
	return current().attackButton;
}

bool secro::Controller::attackReleased() const
{
	return !current().attackButton && prev().attackButton;
}

bool secro::Controller::specialPressed() const
{
	return current().specialButton && !prev().specialButton;
}

bool secro::Controller::specialHeld() const
{
	return current().specialButton;
}

bool secro::Controller::specialReleased() const
{
	return !current().specialButton && prev().specialButton;
}

bool secro::Controller::blockPressed() const
{
	return current().shieldButton && !prev().shieldButton;
}

bool secro::Controller::blockHeld() const
{
	return current().shieldButton;
}

bool secro::Controller::blockReleased() const
{
	return !current().shieldButton && prev().shieldButton;
}

bool secro::Controller::grabPressed() const
{
	return current().grabButton && !prev().grabButton;
}

bool secro::Controller::grabHeld() const
{
	return current().grabButton;
}

bool secro::Controller::grabReleased() const
{
	return !current().grabButton && prev().grabButton;
}

bool secro::Controller::jumpPressed() const
{
	return (current().jumpButton && !prev().jumpButton);
}

bool secro::Controller::jumpHeld() const
{
	return current().jumpButton || current().jumpButton;
}

bool secro::Controller::jumpReleased() const
{
	return !current().jumpButton && prev().jumpButton;
}

secro::Controller::Controller(int playerIndexx, int index, bool keyboard)
{
	controllerIndex = index;
	useKeyboard = keyboard;
	playerIndex = playerIndexx;
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

ControllerSettings & secro::Controller::getSettings()
{
	return settings;
}

bool secro::Controller::findAxis(AxisConfig & axis)
{
	for (size_t i = 0; i < 8; ++i)
	{
		float axisVal = sf::Joystick::getAxisPosition(controllerIndex, (sf::Joystick::Axis)i);
		if (axisVal > 90.f)
		{
			axis.axis = i;
			axis.scale = 1.f;
			return true;
		}
		else if (axisVal < -90.f)
		{
			axis.axis = i;
			axis.scale = -1.f;
			return true;
		}
	}
	return false;
}

bool secro::Controller::findButton(ButtonIndex & button)
{
	unsigned int count = sf::Joystick::getButtonCount(controllerIndex);
	for (unsigned int i = 0; i < count; ++i)
	{
		if (sf::Joystick::isButtonPressed(controllerIndex, i))
		{
			button = i;
			return true;
		}
	}
	return false;
}

int secro::Controller::getPlayerIndex()
{
	return playerIndex;
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

std::shared_ptr<Controller> secro::Controller::createController(int playerIndex, int index, bool keyboard)
{
	class make_shared_enabler : public Controller {
	public:
		make_shared_enabler(int playerIndex, int index, bool keyboard) : Controller(playerIndex, index, keyboard) {}
	};

	return std::make_shared<make_shared_enabler>(playerIndex, index, keyboard);
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
	if (playerIndex != -1 && sf::Joystick::isConnected(controllerIndex))
	{
		Input i;

		//buttons
		i.specialButton = sf::Joystick::isButtonPressed(controllerIndex, settings.specialButton);
		i.attackButton = sf::Joystick::isButtonPressed(controllerIndex, settings.attackButton);
		i.jumpButton = sf::Joystick::isButtonPressed(controllerIndex, settings.jumpButton);
		i.grabButton = sf::Joystick::isButtonPressed(controllerIndex, settings.grabButton);
		i.startButton = sf::Joystick::isButtonPressed(controllerIndex, settings.startButton);
		i.selectButton = sf::Joystick::isButtonPressed(controllerIndex, settings.selectButton);

		//triggers (need to be tested)
		i.shieldButton = sf::Joystick::getAxisPosition(controllerIndex, (sf::Joystick::Axis)settings.shieldButton.axis) < (20.f * settings.shieldButton.scale);

		//joysticks
		i.leftStick.x = sf::Joystick::getAxisPosition(controllerIndex, (sf::Joystick::Axis)settings.leftStick.x.axis) * settings.leftStick.x.scale;
		i.leftStick.y = sf::Joystick::getAxisPosition(controllerIndex, (sf::Joystick::Axis)settings.leftStick.y.axis) * settings.leftStick.y.scale;

		//joysticks
		i.rightStick.x = sf::Joystick::getAxisPosition(controllerIndex, (sf::Joystick::Axis)settings.rightStick.x.axis) * settings.rightStick.x.scale;
		i.rightStick.y = sf::Joystick::getAxisPosition(controllerIndex, (sf::Joystick::Axis)settings.rightStick.y.axis) * settings.rightStick.y.scale;

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
