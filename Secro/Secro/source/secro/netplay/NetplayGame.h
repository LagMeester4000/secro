#pragma once
#include <SFML/Network.hpp>
#include <memory>
#include "secro/IGame.h"

namespace secro {
	class InputManager;

	class NetplayGame : IGame {
	public:
		enum ConnectionType {
			Client,//connecter
			Server,//host
		};

	public:
		NetplayGame();

		//attempt to make a connection
		//this will be using the command line
		void setup();

		void update(float deltaTime) override;
		void render(sf::RenderWindow& window) override;

		//actually run the game update
		void simulate(float deltaTime);

	private:
		//input manager 
		std::shared_ptr<InputManager> input;

		//actual game to be simulated
		std::shared_ptr<Game> game;

	private: //netcode
		//counter that keeps track of how many frames have passed
		unsigned int frameCounter;

		//the index of the controller that this application is controlling
		int localController;

		//the type of connection that this application uses
		ConnectionType connectionType;

		//the socket to communicate over
		sf::TcpSocket socket;

		//the listener for the server
		sf::TcpListener listener;

		//the socket used for sending information
		sf::Packet sendPacket;

		//the socket used for receiving information
		sf::Packet receivePacket;

	private:
		//the size of the input buffer in frames
		int inputBufferSize;
	};
}