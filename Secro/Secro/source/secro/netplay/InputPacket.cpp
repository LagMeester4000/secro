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
	return packet << input.aButton << input.bButton << input.leftStick.x << input.leftStick.y << input.rightStick.x << input.rightStick.y
		<< input.xButton << input.yButton << input.lTrigger1 << input.lTrigger2 << input.rTrigger1 << input.rTrigger2 << input.select
		<< input.start;
}

sf::Packet & operator>>(sf::Packet & packet, secro::ControllerInput & input)
{
	return packet >> input.aButton >> input.bButton >> input.leftStick.x >> input.leftStick.y >> input.rightStick.x >> input.rightStick.y
		>> input.xButton >> input.yButton >> input.lTrigger1 >> input.lTrigger2 >> input.rTrigger1 >> input.rTrigger2 >> input.select
		>> input.start;
}
