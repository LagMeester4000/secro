#include <iostream>
#include <SFML/Network.hpp>

//unsigned short PortNum = 31415;
//
//int main()
//{
//	std::string IPString;
//	std::cout << "Input IP:" << std::endl;
//	std::cin >> IPString;
//	sf::IpAddress IP = sf::IpAddress(IPString.c_str());
//
//	int Choose;
//	std::cout << "0 for server, 1 for client" << std::endl;
//	std::cin >> Choose;
//
//	if (Choose)
//	{
//		// Client
//		sf::UdpSocket Sock;
//		//if (Sock.bind(PortNum/*, IP*/) == sf::Socket::Done)
//		{
//			for (int I = 0; I < 3; ++I)
//			{
//				sf::Packet Pack;
//				Pack << I; 
//				auto Stat = Sock.send(Pack, IP, PortNum);
//			}
//
//			sf::Packet RecPack;
//			sf::IpAddress OtherIP;
//			unsigned short OtherPort;
//			if (Sock.receive(RecPack, OtherIP, OtherPort) == sf::Socket::Status::Done)
//			{
//				std::string RecStr;
//				RecPack >> RecStr;
//				std::cout << RecStr << std::endl;
//			}
//		}
//		//else
//		//{
//		//	std::cout << "failed to bind on client" << std::endl;
//		//}
//
//	}
//	else
//	{
//		// Server
//		sf::UdpSocket Sock;
//		if (Sock.bind(PortNum/*, IP*/) == sf::Socket::Done)
//		{
//			Sock.setBlocking(false);
//
//			int Leave = 1;
//			std::cout << "1 to continue, 0 to exit" << std::endl;
//			std::cin >> Leave;
//			while (Leave == 1)
//			{
//				//sf::Uint32
//				sf::Packet Pack;
//				sf::IpAddress OtherIP;
//				unsigned short OtherPort;
//				while (Sock.receive(Pack, OtherIP, OtherPort) == sf::Socket::Done)
//				{
//					PortNum = OtherPort;
//					IP = OtherIP;
//					int PrintInt;
//					Pack >> PrintInt;
//					std::cout << PrintInt << std::endl;
//
//					Pack.clear();
//				}
//
//				std::cout << "1 to retry, 0 to exit" << std::endl;
//				std::cin >> Leave;
//			}
//
//			sf::Packet BackPack;
//			std::string BackStr = "YAYO WORKO";
//			BackPack << BackStr;
//			Sock.send(BackPack, IP, PortNum);
//		}
//	}
//
//	system("pause");
//	return 1;
//}

/*
#include <string>
#include <chrono>
#include <thread>
#include "KraNet/KraNetSession.h"

using namespace kra::net;

unsigned short PortNum = 31415;
int LoopLen = 900;

void MyUpdate(void*, KraNetInput I1, KraNetInput I2)
{
	std::cout << "Did a frame" << std::endl;
}

int main()
{
	KraNetSession Session(nullptr, &MyUpdate);

	std::cout << "0 for host, 1 for join" << std::endl;
	int C;
	std::cin >> C;

	if (C == 0)
	{
		// Host
		if (!Session.ListenConnection(PortNum))
		{
			std::cout << "ERROR: failed to listen for connection" << std::endl;
		}
	}
	else if (C == 1)
	{
		// Join
		std::cout << "Ip to connect to:" << std::endl;
		std::string IpStr;
		std::cin >> IpStr;
		sf::IpAddress Ip(IpStr.c_str());
		if (!Session.StartConnection(Ip, PortNum))
		{
			std::cout << "ERROR: failed to join host" << std::endl;
		}
	}

	// Do loop
	for (auto I = 0; I < LoopLen; ++I)
	{
		KraNetInput Push;
		Push.i0 = 25;
		Session.Update(Push);

		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}
}*/