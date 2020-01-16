#pragma once
#include <SFML/Network/UdpSocket.hpp>
#include <SFML/System.hpp>
#include <memory>

namespace kra {
	namespace net {
		// Peer class that does the peer to peer networking
		class UDPPeer {
		public:
			enum class ConnectionState {
				Disconnected,
				Connecting,
				Connected
			};

		private: // Constructor
			UDPPeer();

		public:
			// Can only create on heap
			static std::shared_ptr<UDPPeer> Create();

		private:
			// State of the connection
			ConnectionState State;

			// Main socket to connect and receive data through
			sf::UdpSocket Sock;

			// The ip to connect to
			sf::IpAddress OtherIp;

			// Port for connection
			unsigned short OtherPort;

		private: // Thread stuff
			sf::Thread Thread;
			sf::Mutex Lock;
		};
	}
}