#pragma once
#include <vector>
#include <stdint.h>
#include "KraNetInput.h"

namespace kra {
	namespace net {
		struct InputPair {
			KraNetInput Local, Remote;
		};

		struct NetInputBufferElement {
			InputPair Input;
			bool LocalValid = false,
				RemoteValid = false;
		};

		// Circlebuffer for storing inputs by frame
		class NetInputBuffer {
		public:
			using FrameT = uint32_t;
		public:
			NetInputBuffer();

			// Reset the buffer
			// Sets CurrentFrame to 0
			void Reset();

			// Check if you can advance the buffer by one frame
			bool CanAdvanceConfirmedFrame() const;

			// Check if you can advance the buffer by one frame locally
			bool CanAdvanceLocalGameplayFrame() const;

			// Check if a frame has been missed
			bool HasRemoteFrameBeenMissed() const;

			// Advance the buffer by one frame
			// After which the previous frame can be deleted
			bool AdvanceConfirmedFrame();

			// Advance the buffer by one frame
			bool AdvanceLocalGameplayFrame();

			// Get input for current gameplay frame
			const InputPair& GetLocalGameplayFrame() const;

			// Make input for current gameplay frame
			// If the input for the other player has not arrived, duplicate the previous one
			InputPair MakeLocalGameplayFrame() const;
			InputPair MakeLocalGameplayFrame(FrameT Frame) const;

			// Insert input from network
			void InsertNetworkFrame(FrameT NewFrame, KraNetInput Input);

			// Insert local input
			void InsertLocalFrame(FrameT Delay, KraNetInput Input);

			// Initialize the local frames to account for buffer
			// Should only be used once at the start of the session
			void InitializeLocalFrames(FrameT Delay);

			// Set the size of the input buffer
			// Should only be used once at the start of the program
			void SetSize(size_t NewSize);

			// Get an input from the buffer
			const InputPair& GetInput(FrameT Frame) const;

			// Check if the frame is valid
			// Valid meaning that the frame still exists in the buffer and is initialized
			bool IsValid(FrameT Frame) const;

			// Check if the frame is in range of the buffer
			bool IsInRange(FrameT Frame) const;

			// Check if old frame can be accessed
			bool CanAccessOldFrame(FrameT Frame) const;

			// Returns the lowest accessible frame in the buffer
			FrameT LowestFrame() const;
			FrameT GetGameplayFrameIndex() const;
			FrameT GetLastConfirmedFrameIndex() const;

			// Make a sendable buffer
			void MakeSendableInputBuffer(std::vector<KraNetInput>& Buff, FrameT& StartingFrame) const;
			void MakeSendableMissedInputBuffer(FrameT DesiredFrame, std::vector<KraNetInput>& Buff, FrameT& StartingFrame) const;

			// Read a received buffer
			void ReadReceivedInputBuffer(const std::vector<KraNetInput>& Buff, FrameT StartingFrame);

		private:
			// Returns wether the buffer is initialized
			bool BufferValid() const;

			// Resize the Inputs array but move the elements to their new correct spot
			void ResizeAndFit(size_t NewSize);

			// Get a reference to an element using a frame (does modulo)
			NetInputBufferElement& Access(size_t Frame);
			const NetInputBufferElement& AccessConst(size_t Frame) const;
			static NetInputBufferElement& Access(std::vector<NetInputBufferElement>& Inputs, size_t Frame);

			// Helper
			static FrameT Max(FrameT f1, FrameT f2);

		private:
			std::vector<NetInputBufferElement> Inputs;

			FrameT CurrentGameplayFrame;
			FrameT LastConfirmedFrame;

			FrameT MaxLocalFrame;
			FrameT MaxNetworkFrame;

		private:
			FrameT MaxRollbackFrames;
		};
	}
}