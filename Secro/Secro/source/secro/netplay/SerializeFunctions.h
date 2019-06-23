#pragma once
#include "RawSerializeBuffer.h"

namespace secro {
	template<typename T>
	void netSerSave(T& val, RawSerializeBuffer& buff)
	{
		buff.save(val);
	}

	template<typename T>
	void netSerLoad(T& val, RawSerializeBuffer& buff)
	{
		buff.load(val);
	}

	template<typename T>
	void netSerSave(std::vector<T>& vec, RawSerializeBuffer& buff)
	{
		size_t size = vec.size();
		buff.save(size);
		for (auto& it : vec)
		{
			netSerSave(it, buff);
		}
	}

	template<typename T>
	void netSerLoad(std::vector<T>& vec, RawSerializeBuffer& buff)
	{
		size_t size;
		buff.load(size);
		vec.reserve(size);
		for (size_t i = 0; i < size; ++i)
		{
			T it;
			netSerLoad(it, buff);
			vec.push_back(it);
		}
	}
}