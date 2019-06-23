#pragma once
#include <cstdint>
#include <bitset>

namespace secro {
	#pragma pack(push, 1)
	struct CompressedInput {
		enum ButtonType {
			//right stick
			RightIsNotNullX = 0,
			RightIsPlusX,
			RightIsNotNullY,
			RightIsPlusY,

			//buttons
			Attack,
			Special,
			Jump,
			Shield,
			Grab,
			Start,
			Select
		};

		union I {
			I() {}
			~I() {}

			//whole integer
			int32_t raw = 0;

			//broken down
			struct {
				//uint8_t other;
				uint8_t rightIsNotNullX : 1;
				uint8_t rightIsPlusX : 1;
				uint8_t rightIsNotNullY : 1;
				uint8_t rightIsPlusY : 1;
				uint8_t attack : 1;
				uint8_t special : 1;
				uint8_t jump : 1;
				uint8_t shield : 1;
				uint8_t grab : 1;
				uint8_t start : 1;
				uint8_t select : 1;
				int8_t leftX;
				int8_t leftY;
			} broken;
		} input;

		inline static float toAxis(int8_t val)
		{
			return (float)val / 127.f;
		}

		inline static float toAxis(bool isNotNull, bool isPlus)
		{
			if (!isNotNull)
				return 0.f;
			return isPlus ? 1.f : -1.f;
		}

		inline static int8_t compressAxis(float axis)
		{
			int ret = axis * 127.f;
			if (ret > 127)
				return 127;
			else if (ret < -127)
				return -127;
			return (int8_t)ret;
		}
	};
	#pragma pack(pop)
}