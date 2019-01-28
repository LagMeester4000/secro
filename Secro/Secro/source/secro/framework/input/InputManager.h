#pragma once
#include <vector>
#include "Controller.h"

namespace secro {
	class InputManager {
	public:
		InputManager();

		//initialize the controllers
		void init(int amountOfControllers);

		//update the controller inputs
		void update();

		//get a controller associated with the player number
		std::shared_ptr<Controller> getController(int index);

	private:
		std::vector<std::shared_ptr<Controller>> controllers;
	};
}