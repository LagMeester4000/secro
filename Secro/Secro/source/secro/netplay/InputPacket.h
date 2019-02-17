#pragma once
#include "secro/framework/input/Controller.h"

namespace sf {
	class Packet;
}

namespace secro {
	struct InputPacket {
		int frame;
		int previousSentFrame;
		ControllerInput input;
	};
}

sf::Packet& operator<<(sf::Packet& packet, const secro::InputPacket& input);
sf::Packet& operator>>(sf::Packet& packet, secro::InputPacket& input);

sf::Packet& operator<<(sf::Packet& packet, const secro::ControllerInput& input);
sf::Packet& operator>>(sf::Packet& packet, secro::ControllerInput& input);