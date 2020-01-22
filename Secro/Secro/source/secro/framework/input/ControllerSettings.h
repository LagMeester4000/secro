#pragma once
#include <SFML/Window/Joystick.hpp>
#include <variant>

namespace secro {
	using ButtonIndex = unsigned int;

	struct AxisConfig {
		ButtonIndex axis;
		float scale; //either 1 or -1
	};

	struct JoystickConfig {
		AxisConfig x, y;
	};

	struct ControllerSettings {
		JoystickConfig leftStick = { { sf::Joystick::Axis::X, 1.f }, { sf::Joystick::Axis::Y, 1.f } }; //movement
		JoystickConfig rightStick = { { sf::Joystick::Axis::U, 1.f }, { sf::Joystick::Axis::V, 1.f } }; //attack

		ButtonIndex attackButton = 1;
		ButtonIndex specialButton = 0;
		ButtonIndex jumpButton = 3;

		ButtonIndex grabButton = 5;
		AxisConfig shieldButton = { sf::Joystick::Axis::Z, 1.f };

		ButtonIndex startButton = 8;
		ButtonIndex selectButton = 9;
	};
}