#include "NetplayGame.h"
#include "secro/Game.h"
#include "secro/framework/input/InputManager.h"
#include <iostream>

secro::NetplayGame::NetplayGame()
{
	input = std::make_shared<InputManager>();
	game = Game::createGame(input);
}

void secro::NetplayGame::setup()
{
	int isClient;
	std::cout << "is this a client? ";
	std::cin >> isClient;
	std::cout << std::endl;

	if (isClient)
	{
		//client code
		connectionType = Client;

		std::cout << "running client code" << std::endl << "attempting to connect..." << std::endl;

		auto status = socket.
	}
	else
	{
		//server code
		connectionType = Server;

	}
}

void secro::NetplayGame::update(float deltaTime)
{
}

void secro::NetplayGame::render(sf::RenderWindow & window)
{
}

void secro::NetplayGame::simulate(float deltaTime)
{
	frameCounter++;

	//update input for the game before running it
	

	//update the game
	game->update(deltaTime);
}
