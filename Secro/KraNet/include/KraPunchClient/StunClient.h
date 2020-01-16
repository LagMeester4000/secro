#pragma once
#include <SFML/Network.hpp>
#include <memory>
#include <stdint.h>

namespace kra {
	namespace stun {
		class StunClient {
		public:
			enum PacketType {
				// Host connects to stun server, sends back SessionCode uint32_t
				// Server receives: (u32 State)
				// Server sends: (u32 State, u32 SessionCode)
				Host,

				// Client joins with SessionCode uint32_t
				// Server receives: (u32 State, u32 SessionCode)
				// Server sends: (u32 State, string IP, u16 Port)
				Join,

				// Send new Joined address back to host
				// Server receives: not applicable
				// Server sends: (u32 State, string IP, u16 Port)
				UpdateHost,

				// Send packet to keep connection alive. If UpdateHost Has been sent and this packet is still received, send back an UpdateHost packet
				// Server receives: (u32 State, u32 SessionCode)
				// Server sends: (u32 State) OR (u32 State, string IP, u16 Port)
				KeepAlive,

				//Destroy, // Receives SessionCode uint32_t and destroys it

				// Join another client
				JoinClient,

				PACKET_TYPE_MAX,
			};

			enum class ClientType {
				None,
				Host,
				Join,
			};

			enum class ConnectionState {
				Disconnected,
				Connecting, // Trying to get session code
				Joining, // Has IP and port of other client, but is not connected yet
				Hosting, // Has session code, but is not connected to other client
				Connected, // Host or join is connected
			};

		public:
			StunClient();

			// Start hosting
			void StartHost();

			// Join a hosted session using a session code
			void StartJoin(uint32_t SessionCode);

			// Update
			void Update();

			// Check if the client is done getting the other players address
			bool HasOtherAddress() const;

			// Check if the stun is done
			bool Completed() const;

			// Get the session code value
			uint32_t GetSessionCode() const;

			// Print the IP:Port of the other player
			void PrintOtherAddress() const;

			// Get the external IP of the other player
			sf::IpAddress GetOtherIP() const;

			// Get the external port of the other player
			uint16_t GetOtherPort() const;

			// Get the socket
			std::unique_ptr<sf::UdpSocket>& GetSocket();

		private:
			uint32_t SessionCode;
			ClientType Type;
			ConnectionState State;
			//ConnectionState ClientConnectionState;

		private:
			std::unique_ptr<sf::UdpSocket> Sock;
			sf::Packet InPack, OutPack;
			sf::IpAddress OtherIP;
			uint16_t OtherPort;
		};
	}
}