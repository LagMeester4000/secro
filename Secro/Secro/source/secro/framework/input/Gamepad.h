#pragma once
#include <array>
#include <string>

namespace secro {
	//class that maps sf::Joystick to a new interface that maps the buttons/sticks correctly
	class Gamepad {
	public:
		enum Buttons {
			FaceLeft,
			FaceRight,
			FaceUp,
			FaceDown,
			
			LeftShoulder,
			RightShoulder,

			SpecialLeft, //select
			SpecialRight, //start
			
			LeftStickPress,
			RightStickPress,

			BUTTONS_MAX,
		};

		enum Axes {
			LeftStickX,
			LeftStickY,
			RightStickX,
			RightStickY,

			DPadX,
			DPadY,

			LeftTrigger,
			RightTrigger,

			AXES_MAX,
		};

		enum {
			AxisAndButtonsMax = BUTTONS_MAX + AXES_MAX,
		};

		enum MaxVals {
			MaxButton = 32,
			MaxAxis = 8,

			ButtonStart = 0,
			AxisStart = 32,
			TotalMax = 40,
		};

	public:
		Gamepad();
		Gamepad(int index);

	public: //set controller
		//sets index that the gamepad is using
		//also loads the button mappings
		void setPadIndex(int index);
		
		//saves the button mappings to a file located in "controllerMapping.json"
		void saveMappings() const;

		//tries to load button mappings from "controllerMapping.json"
		bool loadMappings();

		//dump input of all the axes and buttons into an array
		//for buttons: value is either 0 or 1, unpressed or pressed
		//for axes: value is either -1, 0, or 1, meaning negative (<-50), 
		//  neutral (>-50, <50), or positive (>50)
		std::array<int, MaxVals::TotalMax> dumpInput();

		//set a button mapping
		void setButtonMapping(Buttons button, int index, bool swapVal = false);

		//set a stick mapping
		void setAxisMapping(Axes axis, int index, bool swapVal);

		//a generic way to set the mapping
		void setMapping(int buttonOrAxisIndex, int index, bool swapVal);

		//get button or axis mapping index
		int getMapping(int buttonOrAxisIndex);

		//get swap value
		bool getMappingSwap(int buttonOrAxisIndex);

		//get the name of the device
		std::string getGamepadName() const;

	public: //get button/axis values
		bool getKeyDown(Buttons button) const;
		float getAxis(Axes axis) const;

	private:
		std::array<int, Buttons::BUTTONS_MAX> buttonMappings;
		std::array<int, Axes::AXES_MAX> axisMappings;
		std::array<bool, AxisAndButtonsMax> axisMappingsSwap; //first x bytes is for buttons, remaining is for axes

		//-1 == not_set
		int controllerIndex = -1;
	};
}