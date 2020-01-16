#pragma once
#include <vector>
#include <chrono>
#include <memory>
#include <SFML/Network.hpp>
#include "NetInputBuffer.h"
#include "NetPacket.h"

namespace kra {
	namespace net {
		class KraNetSession {
		public:
			using Clock = std::chrono::high_resolution_clock;
			using TimePoint = std::chrono::time_point<Clock>;
			
		public:
			KraNetSession(void* ExternalData, void(*UpdateF)(void*, KraNetInput, KraNetInput));
			~KraNetSession();
			
			// Client
			bool StartConnection(sf::IpAddress OtherIp, unsigned short Port);
			uint32_t StartHost();

			// Server
			bool ListenConnection(unsigned short Port);
			void StartJoin(uint32_t SessionCode);

			// Update the inputs
			void Update(KraNetInput LocalInput);

			// Get Host value
			bool IsHost() const;

		public: // Getters
			double GetPing() const;
			sf::IpAddress GetOtherIp() const;
			unsigned short GetOtherPort() const;
			unsigned short GetDelay() const;
			uint32_t GetPingIndex() const;
			int64_t GetLastFrameDifference() const;
			uint32_t GetCurrentFrame() const;

		public: // Setters
			void SetUpdateFunction(void(*F)(void*, KraNetInput, KraNetInput));
			void SetSimulateFunction(void(*F)(void*, KraNetInput, KraNetInput));
			void SetRollbackLoadStateFunction(void(*F)(void*));
			void SetRollbackSaveStateFunction(void(*F)(void*));

		private:
			void Initialize();

			void UpdateLocalGameplay();
			void UpdateSimulateGameplay(NetInputBuffer::FrameT Frame);
			void UpdateLocalInput(KraNetInput LocalInput);
			void SendInput();
			void ReceiveInput();

		private:
			// Check if rollback can be performed
			// Should only be called after new frames are inserted from the network (for performance reasons)
			bool CanRollback() const;

			// Perform rollback
			// Should only be called after new frames are inserted from the network (for performance reasons)
			void Rollback();

		private: // Const data
			// Pointer to some external class/object
			void* External;

			// Function that is called every frame to update the game
			void(*UpdateFunction)(void*, KraNetInput, KraNetInput);

			// Function that is called after rollback
			void(*SimulateFunction)(void*, KraNetInput, KraNetInput);

			// Function that is called when rollback occurs
			// The function should load the last saved state
			void(*RollbackLoadStateFunction)(void*);

			// Function that is called after rollback
			// The function should store the current state (the confirmed frame)
			void(*RollbackSaveStateFunction)(void*);

			// The ip to connect to
			sf::IpAddress OtherIp;

			// Port for connection
			unsigned short OtherPort;

			// The maximum amount of rollback frames
			unsigned short MaxRollbackFrames;
			
			// The input delay in frames
			unsigned short DelayLength;

			// Holds if this peer is the host
			bool Host;

			// Holds if the session is using stun server or not
			bool UseStun = true;

		private: // Non-const data
			NetInputBuffer Inputs;
			std::unique_ptr<sf::UdpSocket> Sock;
			sf::Packet InPacket, OutPacket;
			NetPacket InNetPacket, OutNetPacket;
			stun::StunClient Stun;

			// Ping
			TimePoint LastPing;
			uint32_t PingIndex = 1; // Very important to start at 1
			bool PingDone = false;
			double LastPingTime = 0.0;
			int64_t LastFrameDifference = 0;
		};
	}
}