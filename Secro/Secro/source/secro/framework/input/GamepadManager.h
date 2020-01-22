#pragma once
#include "Gamepad.h"
#include <SFML/Window/Joystick.hpp>
#include <vector>

namespace secro {
	class GamepadManager {
		struct JoyData {
			bool connected = false;
			sf::Joystick::Identification LastId;
			Gamepad pad;
		};

		struct PadInd {
			int index = -1;
			bool used = false;
		};

		struct Members {
			Members();
			std::array<JoyData, 8> pads;
			std::array<int, 8> padMap;
			int padAllocator = 0;
		};

	public:
		//update mapped gamepads
		static void update();
		
		//finds an unused spot in padMap and return its index
		static int createGamepad();

		//get a gamepad from an index value
		static Gamepad& getGamepad(int index);

		//returns wether the gamepad can be used for getting input
		static bool isConnected(int index);

		//returns the connected gamepads
		static std::vector<int> debugGetConnectedGamepads();

		//returns a gamepad from pads buffer
		//please don't use this function 
		static Gamepad& debugGetGamepad(int rawGamepadIndex);

	private:
		//find an unused gamepad (in `pads`) and return its id
		static int allocateGamepad();

		static void tryAttachGamepad(int index);
		static void tryRemoveGamepad(int index);

	private:
		static Members members;
	};
}