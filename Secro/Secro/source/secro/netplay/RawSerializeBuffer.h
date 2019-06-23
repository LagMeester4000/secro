#pragma once
#include <vector>
#include <cstring>

namespace secro {
	class RawSerializeBuffer {
	public:
		//write a value to the buffer
		template<typename T>
		void save(T& val)
		{
			if (readIndex + sizeof(T) >= buffer.size())
			{
				buffer.resize(buffer.capacity() * 2 + sizeof(T));
			}
			memcpy_s(&buffer[readIndex], buffer.capacity(), &val, sizeof(T));
			readIndex += sizeof(T);
		}

		//read a value from the buffer
		template<typename T>
		void load(T& val)
		{
			T* fill = (T*)&buffer[readIndex];
			val = *fill;
			readIndex += sizeof(T);
		}

		//resets the reading index
		//required before starting memory read
		void reset()
		{
			readIndex = 0;
		}

		int hash()
		{
			int ret = 0;
			for (size_t i = 0; i < readIndex; ++i)
			{
				ret += buffer[i];
			}
			return ret;
		}

	private:
		std::vector<char> buffer;
		size_t readIndex = 0;
	};
}