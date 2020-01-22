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

	//get joyinfo
	auto joyInfo = sf::Joystick::getIdentification((unsigned int)controllerIndex);
	auto strr = joyInfo.name.toAnsiString();
	std::cout << "Joystick name: \"" << strr <<
		"\"  product-id: " << joyInfo.productId <<
		"  vendor-id: " << joyInfo.vendorId << std::endl;

	//json file to read from and write to
	json read;

	//load in existing json data
	{
		//load json
		std::ifstream file;
		file.open("./controllerMapping.json");

		//check if file exists
		if (!(bool)file)
		{
			return;
		}

		//into JSon
		std::string s((std::istreambuf_iterator<char>(file)),
			std::istreambuf_iterator<char>());
		read = json::parse(s.begin(), s.end());
	}

	//write json
	std::string infAsString = std::to_string(joyInfo.productId);
	auto& find = read[infAsString];

#define TEMPBUTTONMAP(ID) find[#ID] = buttonMappings[ID];\
						  find[#ID "_swap"] = axisMappingsSwap[ID];
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

#define TEMPAXISMAP(ID) find[#ID] = axisMappings[ID]; \
						find[#ID "_swap"] = axisMappingsSwap[ID + Buttons::BUTTONS_MAX];
	TEMPAXISMAP(LeftStickX);
	TEMPAXISMAP(LeftStickY);
	TEMPAXISMAP(RightStickX);
	TEMPAXISMAP(RightStickY);
	TEMPAXISMAP(DPadX);
	TEMPAXISMAP(DPadY);
	TEMPAXISMAP(LeftTrigger);
	TEMPAXISMAP(RightTrigger);

	//write back into file
	{
		std::ofstream File;
		File.open("./controllerMapping.json");
		File << read;
	}
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

#define TEMPBUTTONMAP(ID) buttonMappings[ID] = (*find)[#ID];\
						axisMappingsSwap[ID] = (*find)[#ID "_swap"];
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

#define TEMPAXISMAP(ID) axisMappings[ID] = (*find)[#ID]; \
						axisMappingsSwap[ID + Buttons::BUTTONS_MAX] = (*find)[#ID "_swap"];
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

std::array<int, secro::Gamepad::MaxVals::TotalMax> secro::Gamepad::dumpInput()
{
	std::array<int, MaxVals::TotalMax> ret;

	//set to null
	memset(ret.data(), 0, ret.size() * sizeof(int));

	//check connection
	if (controllerIndex == -1)
		return ret;
	if (!sf::Joystick::isConnected(controllerIndex))
		return ret;

	//look for every button value
	for (unsigned int i = 0; i < MaxVals::MaxButton; ++i)
	{
		ret[i] = (int)sf::Joystick::isButtonPressed((unsigned int)controllerIndex, i);
	}

	//look for every axis value
	for (unsigned int i = MaxVals::AxisStart; i < MaxVals::TotalMax; ++i)
	{
		float val = sf::Joystick::getAxisPosition((unsigned int)controllerIndex,
			(sf::Joystick::Axis)(i - MaxVals::AxisStart));

		//check if the axis value is negative, positive, or neutral
		if (val < -50.f)
			ret[i] = -1;
		else if (val > 50.f)
			ret[i] = 1;
		else
			ret[i] = 0;
	}

	return ret;
}

void secro::Gamepad::setButtonMapping(Buttons button, int index, bool swapVal)
{
	buttonMappings[button] = index;
	axisMappingsSwap[button] = swapVal;
}

void secro::Gamepad::setAxisMapping(Axes axis, int index, bool swapVal)
{
	axisMappings[axis] = index;
	axisMappingsSwap[axis + Buttons::BUTTONS_MAX] = swapVal;
}

void secro::Gamepad::setMapping(int buttonOrAxis, int index, bool swapVal)
{
	if (buttonOrAxis >= Buttons::BUTTONS_MAX)
	{
		//is axis
		axisMappings[buttonOrAxis - Buttons::BUTTONS_MAX] = index;
		axisMappingsSwap[buttonOrAxis] = swapVal;
	}
	else
	{
		//is button
		buttonMappings[buttonOrAxis] = index;
		axisMappingsSwap[buttonOrAxis] = swapVal;
	}
}

int secro::Gamepad::getMapping(int buttonOrAxisIndex)
{
	return buttonOrAxisIndex >= Buttons::BUTTONS_MAX ? 
		axisMappings[buttonOrAxisIndex - Buttons::BUTTONS_MAX] : 
		buttonMappings[buttonOrAxisIndex];
}

bool secro::Gamepad::getMappingSwap(int buttonOrAxisIndex)
{
	return axisMappingsSwap[buttonOrAxisIndex];
}

std::string secro::Gamepad::getGamepadName() const
{
	if (controllerIndex == -1)
		return "";

	auto joyInfo = sf::Joystick::getIdentification((unsigned int)controllerIndex);
	return joyInfo.name.toAnsiString();
}


bool secro::Gamepad::getKeyDown(Buttons button) const
{
	if (controllerIndex == -1)
		return false;
	int buttonIndex = buttonMappings[button];
	if (buttonIndex == -1)
		return false;

	// Check for Button or Stick
	if (buttonIndex >= MaxVals::AxisStart)
	{
		// Stick
		if (axisMappingsSwap[button])
		{
			return sf::Joystick::getAxisPosition((unsigned int)controllerIndex,
				(sf::Joystick::Axis)(buttonIndex - MaxVals::AxisStart)) < -50.f;
		}
		else
		{
			return sf::Joystick::getAxisPosition((unsigned int)controllerIndex,
				(sf::Joystick::Axis)(buttonIndex - MaxVals::AxisStart)) > 50.f;
		}
	}
	else
	{
		// Button
		if (axisMappingsSwap[button])
		{
			return !sf::Joystick::isButtonPressed((unsigned int)controllerIndex,
				(unsigned int)buttonIndex);
		}
		else
		{
			return sf::Joystick::isButtonPressed((unsigned int)controllerIndex,
				(unsigned int)buttonIndex);
		}
	}
}

float secro::Gamepad::getAxis(Axes axis) const
{
	if (controllerIndex == -1)
		return false;
	int axisIndex = axisMappings[axis];
	if (axisIndex == -1)
		return false;

	// Check for Button or Stick
	if (axisIndex >= MaxVals::AxisStart)
	{
		// Stick
		if (axisMappingsSwap[axis + Buttons::BUTTONS_MAX])
		{
			return -sf::Joystick::getAxisPosition((unsigned int)controllerIndex,
				(sf::Joystick::Axis)(axisIndex - MaxVals::AxisStart));
		}
		else
		{
			return sf::Joystick::getAxisPosition((unsigned int)controllerIndex,
				(sf::Joystick::Axis)(axisIndex - MaxVals::AxisStart));
		}
	}
	else
	{
		// Button
		if (axisMappingsSwap[axis + Buttons::BUTTONS_MAX])
		{
			return (float)(sf::Joystick::isButtonPressed((unsigned int)controllerIndex,
				(unsigned int)axisIndex)) * -100.f;
		}
		else
		{
			return (float)(sf::Joystick::isButtonPressed((unsigned int)controllerIndex,
				(unsigned int)axisIndex)) * 100.f;
		}
	}
}
