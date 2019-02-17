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

		//get the controllers, but a const reference so you can't change them
		const std::vector<std::shared_ptr<Controller>>& getControllers();

	private:
		std::vector<std::shared_ptr<Controller>> controllers;
		
	private: //netcode
		std::vector<std::vector<typename Controller::Input>> inputBuffer;
		int inputBufferSize;
	};
}