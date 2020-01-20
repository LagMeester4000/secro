#include "GamepadManager.h"

using namespace secro;

GamepadManager::Members GamepadManager::members = GamepadManager::Members();

void secro::GamepadManager::update()
{
	for (unsigned int i = 0; i < 8; ++i)
	{
		auto& it = members.pads[i];
		if (sf::Joystick::isConnected(i))
		{
			//check if pad was previously disconnected
			if (!it.connected)
			{
				it.connected = true;
				it.LastId = sf::Joystick::getIdentification(i);
				it.pad.setPadIndex((int)i);
				tryAttachGamepad(i);
			}
		}
		else
		{
			//check if pad was previously connected
			if (it.connected)
			{
				it.connected = false;
				it.pad.setPadIndex(-1);
				tryRemoveGamepad(i);
			}
		}
	}

	////link up controllers possibly?
	//for (int i = 0; i < 8; ++i)
	//{
	//	auto& it = members.padMap[i];
	//
	//	if (it == -1)
	//		continue;
	//
	//	auto& itJoy = members.pads[it];
	//	if (!itJoy.connected)
	//	{
	//		it = allocateGamepad();
	//	}
	//}
}

int secro::GamepadManager::createGamepad()
{
	if (members.padAllocator >= 8)
		return -1;
	int newPad = members.padAllocator;
	++members.padAllocator;
	members.padMap[newPad] = allocateGamepad();
	return newPad;
}

int secro::GamepadManager::allocateGamepad()
{
	for (int i = 0; i < 8; ++i)
	{
		auto& it = members.pads[i];
		if (!it.connected)
			continue;

		bool doContinue = false;
		for (int j = 0; j < 8; ++j)
		{
			if (members.padMap[j] == i)
			{
				doContinue = true;
				break;
			}
		}
		if (doContinue)
			continue;

		//new gamepad found!
		return i;
	}

	return -1;
}

void secro::GamepadManager::tryAttachGamepad(int index)
{
	for (int i = 0; i < members.padAllocator; ++i)
	{
		auto& it = members.padMap[i];
		if (it == -1)
		{
			it = index;
			return; //make sure it is not assigned twice
		}
	}
}

void secro::GamepadManager::tryRemoveGamepad(int index)
{
	for (int i = 0; i < members.padAllocator; ++i)
	{
		auto& it = members.padMap[i];
		if (it == index)
		{
			it = -1;
			//it can unassign multiple times, as a safety measure
		}
	}
}

Gamepad & secro::GamepadManager::getGamepad(int index)
{
	return members.pads[members.padMap[index]].pad;
}

bool secro::GamepadManager::isConnected(int index)
{
	if (index == -1)
		return false;
	if (members.padMap[index] == -1)
		return false;
	return members.pads[members.padMap[index]].connected;
}

secro::GamepadManager::Members::Members()
{
	for (auto& it : padMap)
		it = -1;
}
