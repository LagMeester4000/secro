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

	public:
		Gamepad();
		Gamepad(int index);

	public: //set controller
		//sets index that the gamepad is using
		//also loads the button mappings
		void setPadIndex(int index);
		
		//saves the button mappings to a file located in "controllerMapping/"
		void saveMappings() const;

		//tries to load button mappings from "controllerMapping/"
		bool loadMappings();

	public: //get button/axis values
		bool getKeyDown(Buttons button) const;
		float getAxis(Axes axis) const;

	private:
		std::array<int, Buttons::BUTTONS_MAX> buttonMappings;
		std::array<int, Axes::AXES_MAX> axisMappings;

		//-1 == not_set
		int controllerIndex = -1;
	};
}