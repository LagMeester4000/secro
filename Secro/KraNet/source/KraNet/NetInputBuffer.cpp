#include "KraNet/NetInputBuffer.h"
#include <assert.h>
#include <iostream>

using namespace kra::net;

kra::net::NetInputBuffer::NetInputBuffer()
	: CurrentGameplayFrame(0), LastConfirmedFrame(0), MaxNetworkFrame(0), MaxLocalFrame(0), MaxRollbackFrames(7)
{
}

void kra::net::NetInputBuffer::Reset()
{
	CurrentGameplayFrame = 0;
	MaxNetworkFrame = 0;
	MaxLocalFrame = 0;
	LastConfirmedFrame = 0;
	for (auto& It : Inputs)
	{
		It = NetInputBufferElement();
	}
}

bool kra::net::NetInputBuffer::CanAdvanceConfirmedFrame() const
{
	return IsValid(LastConfirmedFrame + 1) && LastConfirmedFrame < CurrentGameplayFrame;
}

bool kra::net::NetInputBuffer::CanAdvanceLocalGameplayFrame() const
{
	auto& F = AccessConst(CurrentGameplayFrame);

#if _DEBUG
	if (!(CurrentGameplayFrame <= LastConfirmedFrame + MaxRollbackFrames))
	{
		std::cout << "Could not advance local gameplay frame because last confirmed frame is behind" << std::endl;

		for (FrameT I = 0; I < 8; ++I)
		{
			auto& It = AccessConst(LastConfirmedFrame + I);
			std::cout << "[" << I << "] = valid{" << It.RemoteValid << "} frame{" << (LastConfirmedFrame + I) << "}" << std::endl;
		}
	}

	if (!F.LocalValid)
		std::cout << "Could not advance local gameplay frame because the localInput for the current frame is not valid" << std::endl;
#endif

	return (CurrentGameplayFrame <= LastConfirmedFrame + MaxRollbackFrames) &&
		F.LocalValid;
}

bool kra::net::NetInputBuffer::HasRemoteFrameBeenMissed() const
{
	//return false;
	if (!CanAdvanceConfirmedFrame())
	{
		for (FrameT I = LastConfirmedFrame + 1; I < CurrentGameplayFrame; ++I)
		{
			auto& F = AccessConst(I);
			if (F.RemoteValid)
			{
				return true;
			}
		}
	}
	return false;
}

bool kra::net::NetInputBuffer::AdvanceConfirmedFrame()
{
	if (!IsValid(LastConfirmedFrame + 1))
		return false;

	// Invalidate new element
	//Inputs[(size_t)LastConfirmedFrame % Inputs.size()] = NetInputBufferElement();

	LastConfirmedFrame++;

	return true;
}

bool kra::net::NetInputBuffer::AdvanceLocalGameplayFrame()
{
	if (!CanAdvanceLocalGameplayFrame())
		return false;

	CurrentGameplayFrame++;

	return false;
}

const InputPair & kra::net::NetInputBuffer::GetLocalGameplayFrame() const
{
	return AccessConst((size_t)CurrentGameplayFrame).Input;
}

InputPair kra::net::NetInputBuffer::MakeLocalGameplayFrame() const
{
	auto& F = AccessConst(CurrentGameplayFrame);
	InputPair Ret;
	Ret.Local = F.Input.Local;

	if (F.RemoteValid)
	{
		Ret.Remote = F.Input.Remote;
	}
	else
	{
		auto& FN = AccessConst(LastConfirmedFrame);
		Ret.Remote = FN.Input.Remote;
	}

	return Ret;
}

InputPair kra::net::NetInputBuffer::MakeLocalGameplayFrame(FrameT Frame) const
{
	auto& F = AccessConst(Frame);
	InputPair Ret;
	Ret.Local = F.Input.Local;

	if (F.RemoteValid)
	{
		Ret.Remote = F.Input.Remote;
	}
	else
	{
		auto& FN = AccessConst(LastConfirmedFrame);
		Ret.Remote = FN.Input.Remote;
	}

	return Ret;
}

void kra::net::NetInputBuffer::InsertNetworkFrame(FrameT NewFrame, KraNetInput Input)
{
	assert(NewFrame >= LowestFrame());

	FrameT NewSize = NewFrame - LowestFrame() + 1;
	if (NewSize > Inputs.size())
	{
		ResizeAndFit((size_t)NewSize);
	}

	if (NewFrame > MaxNetworkFrame)
	{
		if (NewFrame > Max(MaxNetworkFrame, MaxLocalFrame))
		{
			Access(NewFrame + 1) = NetInputBufferElement();
		}
		MaxNetworkFrame = NewFrame;
	}

	// Do actual insertion
	auto& In = Access((size_t)NewFrame);
	In.RemoteValid = true;
	In.Input.Remote = Input;
}

void kra::net::NetInputBuffer::InsertLocalFrame(FrameT Delay, KraNetInput Input)
{
	FrameT NewFrame = CurrentGameplayFrame + Delay;
	assert(NewFrame >= LowestFrame());

	FrameT NewSize = NewFrame - LowestFrame() + 1;
	if (NewSize > Inputs.size())
	{
		ResizeAndFit((size_t)NewFrame);
		MaxNetworkFrame = NewFrame;
	}

	if (NewFrame > MaxLocalFrame)
	{
		if (NewFrame > Max(MaxNetworkFrame, MaxLocalFrame))
		{
			Access(NewFrame + 1) = NetInputBufferElement();
		}
		MaxLocalFrame = NewFrame;
	}

	// Do actual insertion
	auto& In = Access((size_t)NewFrame);
	In.LocalValid = true;
	In.Input.Local = Input;

	// If any frames are skipped, fill them with this new input
	/*for (size_t I = NewFrame - 1; I >= CurrentGameplayFrame; --I)
	{
		auto& In = Inputs[(size_t)I];
		if (!In.LocalValid)
		{
			In.LocalValid = true;
			In.Input.Local = Input;
		}
		else
		{
			break;
		}
	}*/
}

void kra::net::NetInputBuffer::InitializeLocalFrames(FrameT Delay)
{
	for (FrameT I = 0; I < Delay; ++I)
	{
		Access(I).LocalValid = true;
	}
	MaxLocalFrame = Delay;
}

void kra::net::NetInputBuffer::SetSize(size_t NewSize)
{
	Reset();
	Inputs.resize(NewSize);
}

const InputPair & kra::net::NetInputBuffer::GetInput(FrameT Frame) const
{
	assert(BufferValid());
	assert(IsInRange(Frame));

	return Inputs[(size_t)Frame % Inputs.size()].Input;
}

bool kra::net::NetInputBuffer::IsValid(FrameT Frame) const
{
	auto& In = Inputs[(size_t)Frame % Inputs.size()];
	return BufferValid() && 
		IsInRange(Frame) &&
		In.LocalValid && In.RemoteValid;
}

bool kra::net::NetInputBuffer::IsInRange(FrameT Frame) const
{
	return Frame <= Max(MaxNetworkFrame, MaxLocalFrame) &&
		Frame >= LowestFrame();
}

bool kra::net::NetInputBuffer::CanAccessOldFrame(FrameT Frame) const
{
	return Frame + (FrameT)Inputs.size() > Max(MaxNetworkFrame, MaxLocalFrame) + 1 &&
		Frame < Max(MaxNetworkFrame, MaxLocalFrame);
}

NetInputBuffer::FrameT kra::net::NetInputBuffer::LowestFrame() const
{
	return LastConfirmedFrame;
}

NetInputBuffer::FrameT kra::net::NetInputBuffer::GetGameplayFrameIndex() const
{
	return CurrentGameplayFrame;
}

NetInputBuffer::FrameT kra::net::NetInputBuffer::GetLastConfirmedFrameIndex() const
{
	return LastConfirmedFrame;
}

void kra::net::NetInputBuffer::MakeSendableInputBuffer(std::vector<KraNetInput>& Buff, FrameT & StartingFrame) const
{
	// Make sure the buffer is cleared
	Buff.clear();

	StartingFrame = LowestFrame();

	FrameT I = LowestFrame();
	const auto* It = &AccessConst(I);
	while (It->LocalValid)
	{
		Buff.push_back(It->Input.Local);

		I++;
		if (!IsInRange(I))
			break;
		It = &AccessConst(I);
	}
}

void kra::net::NetInputBuffer::MakeSendableMissedInputBuffer(FrameT DesiredFrame, std::vector<KraNetInput>& Buff, FrameT & StartingFrame) const
{
	// Make sure the buffer is cleared
	Buff.clear();
	
	// Set starting frame, might be changed later
	StartingFrame = DesiredFrame;

	FrameT I = DesiredFrame;
	FrameT Highest = I + 10;
	while (CanAccessOldFrame(I) && I < Highest)
	{
		const auto* It = &AccessConst(I);
		if (!It->LocalValid)
			break;

		Buff.push_back(It->Input.Local);

		I++;
		if (!(I <= Max(MaxLocalFrame, MaxNetworkFrame)))
			break;
		It = &AccessConst(I);
	}
}

void kra::net::NetInputBuffer::ReadReceivedInputBuffer(const std::vector<KraNetInput>& Buff, FrameT StartingFrame)
{
	for (size_t I = 0; I < Buff.size(); ++I)
	{
		FrameT Frame = StartingFrame + (FrameT)I;
		if (!IsInRange(Frame))
			continue;

		InsertNetworkFrame(Frame, Buff[I]);
	}
}

bool kra::net::NetInputBuffer::BufferValid() const
{
	return Inputs.size() != 0;
}

void kra::net::NetInputBuffer::ResizeAndFit(size_t NewSize)
{
	// NEEDS FIX TO INCLUDE EVERY FRAME INSTEAD OF ONLY USED FRAMES (FOR RESENDING OLD FRAMES)
	assert(false);

	if (NewSize <= Inputs.size())
		return;

	NetInputBufferElement Def = NetInputBufferElement();
	std::vector<NetInputBufferElement> NewVec(NewSize, Def);

	for (FrameT I = LowestFrame(); I <= MaxNetworkFrame; ++I)
	{
		Access(NewVec, (size_t)I) = Access(Inputs, (size_t)I);
	}

	// Move new container into Inputs
	Inputs = std::move(NewVec);
}

NetInputBufferElement & kra::net::NetInputBuffer::Access(size_t Frame)
{
	return Inputs[Frame % Inputs.size()];
}

const NetInputBufferElement & kra::net::NetInputBuffer::AccessConst(size_t Frame) const
{
	return Inputs[Frame % Inputs.size()];
}

NetInputBufferElement & kra::net::NetInputBuffer::Access(std::vector<NetInputBufferElement>& Inputs, size_t Frame)
{
	return Inputs[Frame % Inputs.size()];
}

NetInputBuffer::FrameT kra::net::NetInputBuffer::Max(FrameT f1, FrameT f2)
{
	return f1 > f2 ? f1 : f2;
}
