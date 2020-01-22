#pragma once
#include "Gamepad.h"
#include <vector>
#include <string>

namespace secro {
	class InputMappingUI {
	public:
		InputMappingUI();

		//renders the mapping in imgui
		void render();

		//makes static InputMappingUI and renders it
		static void staticRender();

	private:
		//update the dumpedGamepad array
		//if any value has changed in the array, return the index
		//else return -1
		int updateGamepadDump();

		//set the selectedGamepad value and update other vars
		void setSelectedGamepad(int newPad);

		static std::string getButtonOrAxisName(int buttonOrAxisIndex, bool swapVal);

	private:
		std::array<int, Gamepad::MaxVals::TotalMax> dumpedGamepad;
		int selectedGamepad = -1;
		bool selectingButtonOrAxis = false;
		int selectingButtonOrAxisIndex = -1;

		std::vector<int> gamepads;
		std::vector<std::string> gamepadNames;
	};
}