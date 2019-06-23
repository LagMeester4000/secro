#pragma once
#include <unordered_map>

namespace secro {
	//type T and Tnew satisfies as a pointer (so either a smart pointer or a raw pointer)
	template<typename T, typename Owner, typename Tnew = T>
	class CopyContainer {
	public:
		CopyContainer() {}

		//set the callback function called to copy an object
		void setCreationCallback(Tnew(*callback)(Owner*, T))
		{
			creationCallback = callback;
		}

		//makes a copy of the value and registers the old and new pointer
		void registerOld(T val)
		{
			//create copy
			auto cpy = creationCallback(owner, val);

			auto find = conversionMap.find(val);
			if (conversionMap.end() != find)
			{
				//ERROR, the object was already registered in the map
				return;
			}
			conversionMap[val] = cpy;
		}

		//get the new version of the old object
		Tnew getNew(T old)
		{
			auto find = conversionMap.find(old);
			if (find != conversionMap.end())
			{
				return find->second;
			}
			return nullptr;
		}

	private:
		std::unordered_map<T, Tnew> conversionMap;
		Tnew (*creationCallback)(Owner*, T old);
		Owner* owner;
	};
}