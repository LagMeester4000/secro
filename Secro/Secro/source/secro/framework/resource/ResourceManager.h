#pragma once
#include <memory>
#include <map>
#include <string>
#include <iostream>

namespace secro {
	//generic resource manager class
	template<typename T>
	class ResourceManager {
		struct StaticData {
			std::map<std::string, std::shared_ptr<T>> resources;
		};
	public:
		static std::shared_ptr<T> get(std::string name);

	private:
		static StaticData resources;
	};

	template<typename T>
	typename ResourceManager<T>::StaticData ResourceManager<T>::resources = typename ResourceManager<T>::StaticData();

	template<typename T>
	inline std::shared_ptr<T> ResourceManager<T>::get(std::string name)
	{
		//loadFromFile
		auto result = resources.resources.find(name);
		if (result == resources.resources.end())
		{
			auto shared = std::make_shared<T>();
			if (!shared->loadFromFile(name))
				std::cout << "could not load file \"" << name << "\"!" << std::endl;
			resources.resources[name] = shared;
			return shared;
		}
		return result->second;
	}
}