#include "InputPacket.h"
#include <SFML/Network.hpp>

sf::Packet & operator<<(sf::Packet & packet, const secro::InputPacket & input)
{
	return packet << input.frame << input.previousSentFrame << input.input;
}

sf::Packet & operator>>(sf::Packet & packet, secro::InputPacket & input)
{
	return packet >> input.frame >> input.previousSentFrame >> input.input;
}

sf::Packet & operator<<(sf::Packet & packet, const secro::ControllerInput & input)
{
	return packet << input.attackButton << input.specialButton << input.leftStick.x << input.leftStick.y << input.rightStick.x << input.rightStick.y
		<< input.shieldButton << input.grabButton << input.jumpButton << input.startButton << input.selectButton;
}

sf::Packet & operator>>(sf::Packet & packet, secro::ControllerInput & input)
{
	return packet >> input.attackButton >> input.specialButton >> input.leftStick.x >> input.leftStick.y >> input.rightStick.x >> input.rightStick.y
		>> input.shieldButton >> input.grabButton >> input.jumpButton >> input.startButton >> input.selectButton;
}
