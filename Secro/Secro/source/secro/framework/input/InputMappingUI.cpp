#include "InputMappingUI.h"
#include "GamepadManager.h"
#include "secro/framework/DebugOptions.h"
#include <imgui.h>

secro::InputMappingUI::InputMappingUI()
{
}

void secro::InputMappingUI::render()
{
	if (!DebugOptions::getOptions().enableGamepadEditor)
		return;

	if (ImGui::Begin("Gamepad Input"))
	{
		//update selectable gamepads
		size_t oldGamepadSize = gamepads.size();
		gamepads = GamepadManager::debugGetConnectedGamepads();

		//check if current pad needs to be reset
		if (oldGamepadSize != gamepads.size())
		{
			//maybe reset?
		}

		//update selectable names
		gamepadNames.clear();
		for (auto& it : gamepads)
		{
			auto& pad = GamepadManager::debugGetGamepad(it);
			gamepadNames.push_back(pad.getGamepadName());
		}

		//render gamepad options
		std::string selectedStr =
			(selectedGamepad >= 0 && selectedGamepad < gamepadNames.size()) ?
			gamepadNames[selectedGamepad] :
			"???";
		if (ImGui::BeginCombo("Gamepads", selectedStr.c_str()))
		{
			for (int i = 0; i < gamepadNames.size(); ++i)
			{
				if (ImGui::Selectable(gamepadNames[i].c_str(), i == selectedGamepad))
				{
					setSelectedGamepad(i);
				}
			}

			ImGui::EndCombo();
		}

		//render gamepad button mappings
		if (selectedGamepad != -1)
		{
			Gamepad& currentPad = GamepadManager::debugGetGamepad(selectedGamepad);

#define MAPAXIS(Name)																		\
		{																					\
			int mapId = Gamepad::Name + Gamepad::Buttons::BUTTONS_MAX;						\
			ImGui::PushID(mapId);															\
			ImGui::Text(#Name ": ");														\
			int mapping = currentPad.getMapping(mapId);										\
			bool mappingSwap = currentPad.getMappingSwap(mapId);							\
			std::string mapAsString = getButtonOrAxisName(mapping, mappingSwap);			\
			ImGui::SameLine();																\
			if (ImGui::Button(mapAsString.c_str()))											\
			{																				\
				selectingButtonOrAxis = true;												\
				selectingButtonOrAxisIndex = mapId;											\
			}																				\
			ImGui::PopID();																	\
		}

			//axes
			MAPAXIS(LeftStickX);
			MAPAXIS(LeftStickY);
			MAPAXIS(RightStickX);
			MAPAXIS(RightStickY);
			MAPAXIS(DPadX);
			MAPAXIS(DPadY);
			MAPAXIS(LeftTrigger);
			MAPAXIS(RightTrigger);


#define MAPBUTTON(Name)																		\
		{																					\
			int mapId = Gamepad::Name;														\
			ImGui::PushID(mapId);															\
			ImGui::Text(#Name ": ");														\
			int mapping = currentPad.getMapping(mapId);										\
			bool mappingSwap = currentPad.getMappingSwap(mapId);							\
			std::string mapAsString = getButtonOrAxisName(mapping, mappingSwap);			\
			ImGui::SameLine();																\
			if (ImGui::Button(mapAsString.c_str()))											\
			{																				\
				selectingButtonOrAxis = true;												\
				selectingButtonOrAxisIndex = mapId;											\
			}																				\
			ImGui::PopID();																	\
		}

			//buttons
			MAPBUTTON(FaceLeft);
			MAPBUTTON(FaceRight);
			MAPBUTTON(FaceUp);
			MAPBUTTON(FaceDown);
			MAPBUTTON(LeftShoulder);
			MAPBUTTON(RightShoulder);
			MAPBUTTON(SpecialLeft);
			MAPBUTTON(SpecialRight);
			MAPBUTTON(LeftStickPress);
			MAPBUTTON(RightStickPress);
		}

		//attempt to map new button or axis
		if (selectingButtonOrAxis && selectedGamepad != -1)
		{
			Gamepad& currentPad = GamepadManager::debugGetGamepad(selectedGamepad);

			int find = updateGamepadDump();
			if (find != -1)
			{
				//something moved!
				currentPad.setMapping(selectingButtonOrAxisIndex,
					find,
					dumpedGamepad[find] == -1);

				//stop selection
				selectingButtonOrAxis = false;
				selectingButtonOrAxisIndex = -1;
			}
		}
		else
		{
			updateGamepadDump();
		}


		//save button
		if (ImGui::Button("Save Input Config") && selectedGamepad != -1)
		{
			Gamepad& currentPad = GamepadManager::debugGetGamepad(selectedGamepad);
			currentPad.saveMappings();

			//maybe make the gamepadmanager reload the button mappings for each gamepad?
		}
	}
	ImGui::End();
}

void secro::InputMappingUI::staticRender()
{
	static InputMappingUI ui;
	ui.render();
}

int secro::InputMappingUI::updateGamepadDump()
{
	if (selectedGamepad == -1)
		return -1;

	Gamepad& currentPad = GamepadManager::debugGetGamepad(selectedGamepad);
	auto newDump = currentPad.dumpInput();

	//search for difference
	for (size_t i = 0; i < Gamepad::MaxVals::TotalMax; ++i)
	{
		if (newDump[i] != dumpedGamepad[i])
		{
			//update old dump
			dumpedGamepad = newDump;
			return i;
		}
	}
	
	//update old dump
	dumpedGamepad = newDump;

	return -1;
}

void secro::InputMappingUI::setSelectedGamepad(int newPad)
{
	selectingButtonOrAxis = false;
	selectedGamepad = newPad;
	dumpedGamepad = GamepadManager::debugGetGamepad(newPad).dumpInput();
}

std::string secro::InputMappingUI::getButtonOrAxisName(int buttonOrAxisIndex, bool swapVal)
{
	std::string ret;
	if (swapVal)
		ret = "- ";

	if (buttonOrAxisIndex >= Gamepad::MaxVals::AxisStart)
	{
		//axis
		ret += "Axis ";
		ret += std::to_string(buttonOrAxisIndex - Gamepad::MaxVals::AxisStart);
		return ret;
	}
	else
	{
		//button
		ret += "Button ";
		ret += std::to_string(buttonOrAxisIndex);
		return ret;
	}
}
