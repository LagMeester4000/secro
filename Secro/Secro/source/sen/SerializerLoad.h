#pragma once
#include <string>
#include <type_traits>
#include "rapidjson/document.h"

namespace sen {
	class SerializerLoad {
	public:
		template<typename ... TT>
		void operator()(TT&&... args);

		template<typename T>
		void member(const std::string& name, T& val);

	private:
		template<typename T>
		int ex(T&& val)
		{
			execute(val);
			return 0;
		}

		template<typename T>
		void execute(T&& val)
		{
			if (std::is_base_of_v<sen::Entity, std::remove_reference_t<T>>)
			{
				//writer.StartArray();
				//val.serialize(*this);
				//writer.EndArray();
				//reader.
				///document
			}
			else
			{
				val.serialize(*this);
			}
		}

	public:
		rapidjson::Document document;

	};
	template<typename ...TT>
	inline void SerializerLoad::operator()(TT && ...args)
	{
	}
	template<typename T>
	inline void SerializerLoad::member(const std::string & name, T & val)
	{
	}
}