#pragma once
#include "Controller.h"

namespace secro {
	class PlayerCharacter;

	class InputTestBot {
	public:
		InputTestBot(PlayerCharacter* player, Controller &controller, int input);

		//update function for in the controller
		void update(typename Controller::Input& input);

		//render a screen for adjusting the DI
		void render();

	private:
		PlayerCharacter* player;
		int inputNumber;

		float DI = 0.f;
		bool randomDI;
		bool holdShield = false;
		bool shouldTech = false;
	};
}