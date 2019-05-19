#include "InputTestBot.h"
#include "../DebugOptions.h"
#include "../player/PlayerCharacter.h"
#include <imgui.h>
#include <random>
#include <iostream>

secro::InputTestBot::InputTestBot(PlayerCharacter* player, Controller & controller, int input)
{
	srand(1);
	this->player = player;
	this->inputNumber = input;
	controller.interceptController(std::bind(&InputTestBot::update, this, std::placeholders::_1));
	Controller::Input inp;
	controller.manualUpdate(inp);

}

void secro::InputTestBot::update(typename Controller::Input & input)
{
	input.shieldButton = holdShield;
	
	std::cout << (int)player->getState() << std::endl; 

	if (player->IsInHitstun())
	{
		if (!randomDI)
			input.leftStick.x = DI;
		else
			input.leftStick.x = (float)((rand() % 3 - 1) * 100);

		if (shouldTech)
			input.shieldButton = true;
		else
			input.shieldButton = false;
	}
	else 
	{
		input.leftStick.x = 0.f;
	}
}

void secro::InputTestBot::render()
{
	if (!DebugOptions::getOptions().enableInputEditor)
		return;

	if (ImGui::Begin("CPU Input"))
	{
		ImGui::Checkbox("Hold shield", &holdShield);
		ImGui::Checkbox("Should tech", &shouldTech);
		ImGui::SliderFloat("DI on hit", &DI, -100.f, 100.f);
		ImGui::Checkbox("Random DI", &randomDI);
	}
	ImGui::End();
}
