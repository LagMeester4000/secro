#pragma once
#include <array>
#include <utility>
#include <assert.h>

namespace kra {
	namespace net {
		template<typename T, size_t S>
		class RingBufferStatic {
		public:
			RingBufferStatic()
			{
			}

			void PushBack(T&& Val) 
			{
				size_t TopI = Top();

				if (SizeIndex < Buffer.size())
				{
					SizeIndex++;
				}
				else
				{
					BottomIndex++;
				}

				operator[](TopI) = std::move(Val);
			}

			void PushBack(T& Val)
			{
				size_t TopI = Top();

				if (SizeIndex < Buffer.size())
				{
					SizeIndex++;
				}
				else
				{
					BottomIndex++;
				}

				operator[](TopI) = Val;
			}

			// Returns the lowes usable index
			size_t Bottom() const
			{
				return BottomIndex;
			}

			// Returns Bottom() + BottomIndex
			size_t Top() const
			{
				return BottomIndex + Size();
			}

			// Get the size of the buffer
			size_t Size() const
			{
				return SizeIndex;
			}

			T& operator[](size_t I)
			{
				assert(I >= BottomIndex);
				assert(I < Top());
				return Buffer[I % Size()];
			}

			const T& operator[](size_t I) const
			{
				assert(I >= BottomIndex);
				assert(I < Top());
				return Buffer[I % Size()];
			}

		private:
			std::array<T, S> Buffer;
			size_t BottomIndex = 0;
			size_t SizeIndex = 0;
		};
	}
}