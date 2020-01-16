#pragma once
#include <stdint.h>
#include "KraNetInput.h"
#include "NetInputBuffer.h"
#include "KraPunchClient/StunClient.h"

namespace sf {
	class Packet;
}

namespace kra {
	namespace net {
		enum class NetPacketType : uint8_t {
			Error = stun::StunClient::PACKET_TYPE_MAX, // Don't want to override the old packet type enum
			Setup,
			Input,
			MissedInputInit,
			MissedInputReturn,
			Desync,
		};

		class NetPacket {
		public:
			NetPacket();

			void Load(sf::Packet& Pack);
			void Save(sf::Packet& Pack);

		public:
			NetPacketType Type;

			struct { // Error

			} Error;

			struct { // Setup

			} Setup;

			struct { // Input
				// Ping
				uint32_t PingIndexIn = 0, PingIndexOut = 0;
				uint32_t GameplayFrame = 0;

				// Input
				NetInputBuffer::FrameT StartFrame;
				std::vector<KraNetInput> Input;
			} Input;

			struct { // MissedInputInit
				uint32_t Frame = 0;
			} MissedInputInit;

			struct { // MissedInputInit
				// Input
				NetInputBuffer::FrameT StartFrame;
				std::vector<KraNetInput> Input;
			} MissedInputReturn;

			struct { // Desync

			} Desync;
		};
	}
}