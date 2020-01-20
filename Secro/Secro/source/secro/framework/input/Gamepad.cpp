#include "Gamepad.h"
#include <SFML/Window/Joystick.hpp>
#include <json/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

secro::Gamepad::Gamepad()
{
}

secro::Gamepad::Gamepad(int index)
{
	setPadIndex(index);
}

void secro::Gamepad::setPadIndex(int index)
{
	controllerIndex = index;
	if (index == -1)
		return;

	loadMappings();
}

void secro::Gamepad::saveMappings() const
{
	if (controllerIndex == -1)
		return;
}

bool secro::Gamepad::loadMappings()
{
	if (controllerIndex == -1)
		return false;

	auto joyInfo = sf::Joystick::getIdentification((unsigned int)controllerIndex);
	auto strr = joyInfo.name.toAnsiString();
	std::cout << "Joystick name: \"" << strr << 
		"\"  product-id: " << joyInfo.productId << 
		"  vendor-id: " << joyInfo.vendorId << std::endl;


	///loading
	//load json
	std::ifstream file;
	file.open("./controllerMapping.json");

	//check if file exists
	if (!(bool)file)
	{
		return false;
	}

	//into JSon
	std::string s((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());
	const json read = json::parse(s.begin(), s.end());

	//read json
	std::string infAsString = std::to_string(joyInfo.productId);
	auto find = read.find(infAsString);
	if (find == read.end())
	{
		return false;
	}

#define TEMPBUTTONMAP(ID) buttonMappings[ID] = (*find)[#ID];
	TEMPBUTTONMAP(FaceLeft);
	TEMPBUTTONMAP(FaceRight);
	TEMPBUTTONMAP(FaceUp);
	TEMPBUTTONMAP(FaceDown);
	TEMPBUTTONMAP(LeftShoulder);
	TEMPBUTTONMAP(RightShoulder);
	TEMPBUTTONMAP(SpecialLeft);
	TEMPBUTTONMAP(SpecialRight);
	TEMPBUTTONMAP(LeftStickPress);
	TEMPBUTTONMAP(RightStickPress);

#define TEMPAXISMAP(ID) axisMappings[ID] = (*find)[#ID]; axisMappingsSwap[ID] = (*find)[#ID "_swap"];
	TEMPAXISMAP(LeftStickX);
	TEMPAXISMAP(LeftStickY);
	TEMPAXISMAP(RightStickX);
	TEMPAXISMAP(RightStickY);
	TEMPAXISMAP(DPadX);
	TEMPAXISMAP(DPadY);
	TEMPAXISMAP(LeftTrigger);
	TEMPAXISMAP(RightTrigger);

	return true;
}

bool secro::Gamepad::getKeyDown(Buttons button) const
{
	if (controllerIndex == -1)
		return false;
	int buttonIndex = buttonMappings[button];
	if (buttonIndex == -1)
		return false;
	return sf::Joystick::isButtonPressed((unsigned int)controllerIndex, 
		(unsigned int)buttonIndex);
}

float secro::Gamepad::getAxis(Axes axis) const
{
	if (controllerIndex == -1)
		return false;
	int axisIndex = axisMappings[axis];
	if (axisIndex == -1)
		return false;
	if (axisMappingsSwap[axis])
	{
		return -sf::Joystick::getAxisPosition((unsigned int)controllerIndex,
			(sf::Joystick::Axis)axisIndex);
	}
	else
	{
		return sf::Joystick::getAxisPosition((unsigned int)controllerIndex,
			(sf::Joystick::Axis)axisIndex);
	}
		
}
