#include "KraNet/NetPacket.h"
#include <SFML/Network.hpp>

using namespace kra::net;

void LoadInput(sf::Packet& Pack, KraNetInput& Inp)
{
	Pack >> Inp.i0;
	Pack >> Inp.i1;
	Pack >> Inp.i2;
	Pack >> Inp.i3;
}

void SaveInput(sf::Packet& Pack, KraNetInput& Inp)
{
	Pack << Inp.i0;
	Pack << Inp.i1;
	Pack << Inp.i2;
	Pack << Inp.i3;
}

kra::net::NetPacket::NetPacket()
{
}

void kra::net::NetPacket::Load(sf::Packet & Pack)
{
	uint8_t T;
	Pack >> T;
	Type = (NetPacketType)T;

	switch (Type)
	{
	case NetPacketType::Input:
	{
		// Ping
		Pack >> Input.PingIndexIn;
		Pack >> Input.PingIndexOut;
		Pack >> Input.GameplayFrame;

		// Inputs
		Pack >> Input.StartFrame;
		size_t Size;
		Pack >> Size;

		// Clear of any previous inputs
		Input.Input.clear();

		// Add new inputs
		for (size_t I = 0; I < Size; ++I)
		{
			KraNetInput In;
			LoadInput(Pack, In);
			Input.Input.push_back(In);
		}
	} break;
	case NetPacketType::MissedInputInit:
	{
		Pack >> MissedInputInit.Frame;
	} break;
	case NetPacketType::MissedInputReturn:
	{
		// Inputs
		Pack >> MissedInputReturn.StartFrame;
		size_t Size;
		Pack >> Size;

		// Clear of any previous inputs
		MissedInputReturn.Input.clear();

		// Add new inputs
		for (size_t I = 0; I < Size; ++I)
		{
			KraNetInput In;
			LoadInput(Pack, In);
			MissedInputReturn.Input.push_back(In);
		}
	} break;
	default:
		break;
	}
}

void kra::net::NetPacket::Save(sf::Packet & Pack)
{
	uint8_t T = (uint8_t)Type;
	Pack << T;

	switch (Type)
	{
	case NetPacketType::Input:
	{
		// Ping
		Pack << Input.PingIndexIn;
		Pack << Input.PingIndexOut;
		Pack << Input.GameplayFrame;

		// Inputs
		Pack << Input.StartFrame;
		size_t Size = Input.Input.size();
		Pack << Size;

		// Add new inputs
		for (auto& It : Input.Input)
		{
			SaveInput(Pack, It);
		}
	} break;
	case NetPacketType::MissedInputInit:
	{
		Pack << MissedInputInit.Frame;
	} break;
	case NetPacketType::MissedInputReturn:
	{
		// Inputs
		Pack << MissedInputReturn.StartFrame;
		size_t Size = MissedInputReturn.Input.size();
		Pack << Size;

		// Add new inputs
		for (auto& It : MissedInputReturn.Input)
		{
			SaveInput(Pack, It);
		}
	} break;
	default:
		break;
	}
}
