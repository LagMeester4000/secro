#include "KraNet/KraNetSession.h"
#include <thread>
#include <assert.h>

kra::net::KraNetSession::KraNetSession(void * ExternalData, void(*UpdateF)(void *, KraNetInput, KraNetInput))
	: External(ExternalData), UpdateFunction(UpdateF), MaxRollbackFrames(10), DelayLength(3), Host(false)
{
}

kra::net::KraNetSession::~KraNetSession()
{

}

bool kra::net::KraNetSession::StartConnection(sf::IpAddress OOtherIp, unsigned short PPort)
{
	Sock = std::make_unique<sf::UdpSocket>();
	UseStun = false;

	NetPacket P;
	P.Type = NetPacketType::Setup;
	P.Save(OutPacket);

	if (Sock->send(OutPacket, OOtherIp, PPort) == sf::Socket::Status::Done)
	{
		sf::IpAddress RIp;
		unsigned short RPort;
		if (Sock->receive(InPacket, RIp, RPort) == sf::Socket::Status::Done)
		{
			NetPacket R;
			R.Load(InPacket);

			if (R.Type != NetPacketType::Setup)
				return false;

			Sock->setBlocking(false);
			OtherIp = OOtherIp;
			OtherPort = PPort;
			Initialize();
			Host = false;
			return true;
		}
	}
	
	return false;
}

uint32_t kra::net::KraNetSession::StartHost()
{
	Host = true;
	Stun.StartHost();
	return Stun.GetSessionCode();
}

bool kra::net::KraNetSession::ListenConnection(unsigned short PPort)
{
	Sock = std::make_unique<sf::UdpSocket>();
	UseStun = false;

	if (Sock->bind(PPort) == sf::Socket::Status::Done)
	{
		sf::IpAddress RIp;
		unsigned short RPort;
		if (Sock->receive(InPacket, OtherIp, OtherPort) == sf::Socket::Status::Done)
		{
			NetPacket R;
			R.Load(InPacket);

			if (R.Type != NetPacketType::Setup)
				return false;

			NetPacket P;
			P.Type = NetPacketType::Setup;
			P.Save(OutPacket);
			if (Sock->send(OutPacket, OtherIp, OtherPort) == sf::Socket::Status::Done)
			{
				Sock->setBlocking(false);
				Initialize();
				Host = true;
				return true;
			}
		}
	}
	return false;
}

void kra::net::KraNetSession::StartJoin(uint32_t SessionCode)
{
	Host = false;
	Stun.StartJoin(SessionCode);
}

#include <iostream>
void kra::net::KraNetSession::Update(KraNetInput LocalInput)
{
	// Check if stun should be performed
	if (UseStun && !Stun.Completed())
	{
		Stun.Update();
		if (Stun.Completed())
		{
			Initialize();
			Sock = std::move(Stun.GetSocket());
			OtherIp = Stun.GetOtherIP();
			OtherPort = Stun.GetOtherPort();
		}
	}
	else
	{
		ReceiveInput();

		bool CanAdvance = Inputs.CanAdvanceLocalGameplayFrame();
		if (CanAdvance)
		{
			UpdateLocalInput(LocalInput);
		}

		SendInput();

		if (CanAdvance)
		{
			UpdateLocalGameplay();
		}
		else
		{
			std::cout << "Skipped an input frame" << std::endl;
		}

		// Old order:
		// if possible
		//		UpdateLocalInput
		// ReceiveInput
		// if possible
		//		UpdateLocalGameplay
		// SendInput
	}
}

bool kra::net::KraNetSession::IsHost() const
{
	return Host;
}

double kra::net::KraNetSession::GetPing() const
{
	return LastPingTime;
}

sf::IpAddress kra::net::KraNetSession::GetOtherIp() const
{
	return OtherIp;
}

unsigned short kra::net::KraNetSession::GetOtherPort() const
{
	return OtherPort;
}

unsigned short kra::net::KraNetSession::GetDelay() const
{
	return DelayLength;
}

uint32_t kra::net::KraNetSession::GetPingIndex() const
{
	return PingIndex;
}

int64_t kra::net::KraNetSession::GetLastFrameDifference() const
{
	return LastFrameDifference;
}

uint32_t kra::net::KraNetSession::GetCurrentFrame() const
{
	return Inputs.GetGameplayFrameIndex();
}

void kra::net::KraNetSession::SetUpdateFunction(void(*F)(void *, KraNetInput, KraNetInput))
{
	UpdateFunction = F;
}

void kra::net::KraNetSession::SetSimulateFunction(void(*F)(void *, KraNetInput, KraNetInput))
{
	SimulateFunction = F;
}

void kra::net::KraNetSession::SetRollbackLoadStateFunction(void(*F)(void *))
{
	RollbackLoadStateFunction = F;
}

void kra::net::KraNetSession::SetRollbackSaveStateFunction(void(*F)(void *))
{
	RollbackSaveStateFunction = F;
}

void kra::net::KraNetSession::Initialize()
{
	Inputs.SetSize(30);
	Inputs.InitializeLocalFrames(DelayLength);
	LastPing = Clock::now();

	// Do first save
	if (RollbackSaveStateFunction)
	{
		RollbackSaveStateFunction(External);
	}
}

void kra::net::KraNetSession::UpdateLocalGameplay()
{
	//auto InputFrame = Inputs.GetLocalGameplayFrame();
	auto InputFrame = Inputs.MakeLocalGameplayFrame(Inputs.GetGameplayFrameIndex()); // MIGHT BE REAALY BAD, done to repeat previous input
	
	if (UpdateFunction)
	{
		if (IsHost())
			UpdateFunction(External, InputFrame.Local, InputFrame.Remote);
		else
			UpdateFunction(External, InputFrame.Remote, InputFrame.Local);
	}
}

void kra::net::KraNetSession::UpdateSimulateGameplay(NetInputBuffer::FrameT Frame)
{
	auto InputFrame = Inputs.MakeLocalGameplayFrame(Frame);

	if (SimulateFunction)
	{
		if (IsHost())
			SimulateFunction(External, InputFrame.Local, InputFrame.Remote);
		else
			SimulateFunction(External, InputFrame.Remote, InputFrame.Local);
	}
}

void kra::net::KraNetSession::UpdateLocalInput(KraNetInput LocalInput)
{
	Inputs.InsertLocalFrame(DelayLength, LocalInput);
	Inputs.AdvanceLocalGameplayFrame();
}

double FPS = 60.0;
void kra::net::KraNetSession::SendInput()
{
	// Update ping
	if (PingDone)
	{
		PingDone = false;
		
		auto Now = Clock::now();
		std::chrono::duration<double> Diff = Now - LastPing;
		double PingAsMs = (Diff.count() * 1000.0) / 2.0; // div by 2
		//std::cout << "ping: " << PingAsMs << "ms" << std::endl;
		
		// Set ping var
		LastPingTime = PingAsMs;
		
		int64_t LocalFrame = (int64_t)Inputs.GetGameplayFrameIndex(),
			RemoteFrame = (int64_t)InNetPacket.Input.GameplayFrame;
		int64_t PingAsFrames = (int64_t)(PingAsMs / ((1.0 / FPS) * 1000.0));
		int64_t AdjustedRemoteFrame = RemoteFrame + PingAsFrames;
		int64_t FrameDiff = LocalFrame - AdjustedRemoteFrame;
		//std::cout << "Difference in frames: " << FrameDiff << std::endl;
		//std::cout << "Local Frame: " << LocalFrame << std::endl;

		// Set frame diff var
		LastFrameDifference = FrameDiff;

		// TEMP 
		// use sleeping to get the two machines synced up
		if (FrameDiff >= 2 /*&& IsHost()*/)
		{
			//sleep for half

			// ENABLE THIS FOR SYNCING
			std::this_thread::sleep_for(std::chrono::milliseconds(8 * FrameDiff));
			//std::cout << "Sleeping for " << 8 * FrameDiff << "ms" << std::endl;
		}

		// Reset the time point
		LastPing = Clock::now();
	}	

	// Send main input packet
	{
		// Saving ping test
		OutNetPacket.Input.PingIndexOut = PingIndex;
		OutNetPacket.Input.PingIndexIn = InNetPacket.Input.PingIndexOut;
		OutNetPacket.Input.GameplayFrame = Inputs.GetGameplayFrameIndex();

		// Saving input
		OutNetPacket.Type = NetPacketType::Input;
		Inputs.MakeSendableInputBuffer(OutNetPacket.Input.Input, OutNetPacket.Input.StartFrame);

		// Filling and sending packet
		OutPacket.clear();
		OutNetPacket.Save(OutPacket);
		Sock->send(OutPacket, OtherIp, OtherPort);
	}
	
	// Send MissingInput packet if input is missing
	{
		if (Inputs.HasRemoteFrameBeenMissed())
		{
			// Saving input
			OutNetPacket.Type = NetPacketType::MissedInputInit;
			OutNetPacket.MissedInputInit.Frame = Inputs.LowestFrame() + 1;

			// Filling and sending packet
			OutPacket.clear();
			OutNetPacket.Save(OutPacket);
			Sock->send(OutPacket, OtherIp, OtherPort);

			std::cout << "OH NOES, SENDING PACKET MISSED SIGNAL" << std::endl;
		}
	}
}

void kra::net::KraNetSession::ReceiveInput()
{
	sf::IpAddress Ip;
	unsigned short Port;
	while (Sock->receive(InPacket, Ip, Port) == sf::Socket::Status::Done)
	{
		InNetPacket.Load(InPacket);

		// Read inputs
		if (InNetPacket.Type == NetPacketType::Input)
		{
			// Process ping
			if (InNetPacket.Input.PingIndexIn == PingIndex)
			{
				// Setup the boolean to do the actual work in the SendInput function
				PingDone = true;
				PingIndex++;
			}

			// Process inputs
			Inputs.ReadReceivedInputBuffer(InNetPacket.Input.Input, InNetPacket.Input.StartFrame);
		}
		else if (InNetPacket.Type == NetPacketType::MissedInputInit)
		{
			// Saving input
			OutNetPacket.Type = NetPacketType::MissedInputReturn;
			Inputs.MakeSendableMissedInputBuffer(InNetPacket.MissedInputInit.Frame, OutNetPacket.MissedInputReturn.Input, OutNetPacket.MissedInputReturn.StartFrame);

			// Filling and sending packet
			OutPacket.clear();
			OutNetPacket.Save(OutPacket);
			Sock->send(OutPacket, OtherIp, OtherPort);
		}
		else if (InNetPacket.Type == NetPacketType::MissedInputReturn)
		{
			// Process inputs
			Inputs.ReadReceivedInputBuffer(InNetPacket.MissedInputReturn.Input, InNetPacket.MissedInputReturn.StartFrame);
		}

		InPacket.clear();
	}

	if (CanRollback())
	{
		Rollback();
	}
}

bool kra::net::KraNetSession::CanRollback() const
{
	return Inputs.CanAdvanceConfirmedFrame();
}

void kra::net::KraNetSession::Rollback()
{
	if (!CanRollback())
		return;

	assert(SimulateFunction);
	assert(RollbackLoadStateFunction);
	assert(RollbackSaveStateFunction);

	auto CurrentSimFrame = Inputs.GetLastConfirmedFrameIndex();
	if (CurrentSimFrame >= Inputs.GetGameplayFrameIndex())
		return;

	// Load the last confirmed frame
	RollbackLoadStateFunction(External);

	// Simulate game for confirmed frames (1/2)
	while (Inputs.CanAdvanceConfirmedFrame())
	{
		Inputs.AdvanceConfirmedFrame();

		CurrentSimFrame = Inputs.GetLastConfirmedFrameIndex();
		UpdateSimulateGameplay(CurrentSimFrame);
	}

	// Save the new last confirmed frame
	RollbackSaveStateFunction(External);

	// Perform other simulation
	// One less update because the actual game update is performed later in the gameloop
	assert(CurrentSimFrame <= Inputs.GetGameplayFrameIndex() + 1);
	while (CurrentSimFrame < Inputs.GetGameplayFrameIndex() + 1)
	{
		CurrentSimFrame++;
		UpdateSimulateGameplay(CurrentSimFrame);
	}
}
