#include "Controller.h"
#include <SFML/Window.hpp>
#include <iostream>
#include <math.h>
#include "../detail/PlainVectorMath.h"
#include "secro/framework/input/GamepadManager.h"

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
	return current().jumpButton && !prev().jumpButton;
}

bool secro::Controller::jumpHeld() const
{
	return current().jumpButton;
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

Direction secro::Controller::getDirection(const Joystick & stick)
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

CompressedInput secro::Controller::compressInput(const Input & input)
{
	CompressedInput ret;
	ret.input.raw = 0;

	auto size = sizeof(CompressedInput);
	auto thingSize = sizeof(std::bitset<16>);

	//left stick
	ret.input.broken.leftX = CompressedInput::compressAxis(input.leftStick.x / 100.f);
	ret.input.broken.leftY = CompressedInput::compressAxis(input.leftStick.y / 100.f);
	
	//right stick x
	if (input.rightStick.x > 70.f)
	{
		ret.input.broken.rightIsNotNullX = true;
		ret.input.broken.rightIsPlusX = true;
	}
	else if (input.rightStick.x < -70.f)
	{
		ret.input.broken.rightIsNotNullX = true;
		ret.input.broken.rightIsPlusX = false;
	}
	else
	{
		ret.input.broken.rightIsNotNullX = false;
		ret.input.broken.rightIsPlusX = false;
	}

	//right stick y
	if (input.rightStick.y > 70.f)
	{
		ret.input.broken.rightIsNotNullY = true;
		ret.input.broken.rightIsPlusY = true;
	}
	else if (input.rightStick.y < -70.f)
	{
		ret.input.broken.rightIsNotNullY = true;
		ret.input.broken.rightIsPlusY = false;
	}
	else
	{
		ret.input.broken.rightIsNotNullY = false;
		ret.input.broken.rightIsPlusY = false;
	}

	//buttons
	ret.input.broken.attack = input.attackButton;
	ret.input.broken.special = input.specialButton;
	ret.input.broken.jump = input.jumpButton;
	ret.input.broken.shield = input.shieldButton;
	ret.input.broken.grab = input.grabButton;
	ret.input.broken.start = input.startButton;
	ret.input.broken.select = input.selectButton;

	return ret;
}

typename secro::Controller::Input secro::Controller::uncompressInput(const CompressedInput & input)
{
	Input ret;

	ret.leftStick.x = CompressedInput::toAxis(input.input.broken.leftX) * 100.f;
	ret.leftStick.y = CompressedInput::toAxis(input.input.broken.leftY) * 100.f;

	ret.rightStick.x = CompressedInput::toAxis(
		input.input.broken.rightIsNotNullX, 
		input.input.broken.rightIsPlusX
	) * 100.f;
	ret.rightStick.y = CompressedInput::toAxis(
		input.input.broken.rightIsNotNullY,
		input.input.broken.rightIsPlusY
	) * 100.f;

	ret.attackButton =  input.input.broken.attack;
	ret.specialButton = input.input.broken.special;
	ret.jumpButton =    input.input.broken.jump;
	ret.shieldButton =  input.input.broken.shield;
	ret.grabButton =    input.input.broken.grab;
	ret.startButton =   input.input.broken.start;
	ret.selectButton =  input.input.broken.select;

	return ret;
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
		//if (sf::Joystick::isConnected(controllerIndex))
		{
			Input& i = current();

			i = readInput();

			auto comp = compressInput(i);
			i = uncompressInput(comp);
		}
		//else
		//{
		//	//std::cout << "controller not plugged in" << std::endl;
		//}
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
	if (playerIndex != -1 && GamepadManager::isConnected(controllerIndex)/* && sf::Joystick::isConnected(controllerIndex)*/)
	{
		Input i;
		auto& gamepad = GamepadManager::getGamepad(controllerIndex);

		//buttons
		i.specialButton = gamepad.getKeyDown(Gamepad::FaceDown);//sf::Joystick::isButtonPressed(controllerIndex, settings.specialButton);
		i.attackButton = gamepad.getKeyDown(Gamepad::FaceRight);//sf::Joystick::isButtonPressed(controllerIndex, settings.attackButton);
		i.jumpButton = gamepad.getKeyDown(Gamepad::FaceUp);//sf::Joystick::isButtonPressed(controllerIndex, settings.jumpButton);
		i.grabButton = gamepad.getKeyDown(Gamepad::RightShoulder);//sf::Joystick::isButtonPressed(controllerIndex, settings.grabButton);
		i.startButton = gamepad.getKeyDown(Gamepad::SpecialRight);//sf::Joystick::isButtonPressed(controllerIndex, settings.startButton);
		i.selectButton = gamepad.getKeyDown(Gamepad::SpecialLeft);//sf::Joystick::isButtonPressed(controllerIndex, settings.selectButton);

		//triggers (need to be tested)
		i.shieldButton = gamepad.getAxis(Gamepad::RightTrigger)/*sf::Joystick::getAxisPosition(controllerIndex, (sf::Joystick::Axis)settings.shieldButton.axis)*/ > (20.f * settings.shieldButton.scale);

		//joysticks
		i.leftStick.x = gamepad.getAxis(Gamepad::LeftStickX)/*sf::Joystick::getAxisPosition(controllerIndex, (sf::Joystick::Axis)settings.leftStick.x.axis)*/ * settings.leftStick.x.scale;
		i.leftStick.y = gamepad.getAxis(Gamepad::LeftStickY)/*sf::Joystick::getAxisPosition(controllerIndex, (sf::Joystick::Axis)settings.leftStick.y.axis)*/ * settings.leftStick.y.scale;

		//joysticks
		i.rightStick.x = gamepad.getAxis(Gamepad::RightStickX)/*sf::Joystick::getAxisPosition(controllerIndex, (sf::Joystick::Axis)settings.rightStick.x.axis)*/ * settings.rightStick.x.scale;
		i.rightStick.y = gamepad.getAxis(Gamepad::RightStickY)/*sf::Joystick::getAxisPosition(controllerIndex, (sf::Joystick::Axis)settings.rightStick.y.axis)*/ * settings.rightStick.y.scale;

		return i;
	}
	return Input();
}

void secro::Controller::emptyUpdate()
{
	swapBack();
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

DirectionPress secro::Controller::getMovementPushDirectionExt(bool useOld) const
{
	DirectionPress ret;
	ret.hardPush = false;

	Joystick newJ = current().leftStick;
	Joystick oldJ0 = prev().leftStick;
	Joystick oldJ1 = prev(2).leftStick;

	if (useOld && distance(newJ, oldJ0) < 0.1f)
	{
		newJ = prev().leftStick;
		oldJ0 = prev(2).leftStick;
		oldJ1 = prev(3).leftStick;
	}

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
