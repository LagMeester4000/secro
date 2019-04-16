#pragma once
#include "rapidjson/writer.h"
#include "MemberRef.h"
#include "Entity.h"
#include <type_traits>

namespace sen {
	class Entity;

	class SerializerSave {
	public:
		SerializerSave();

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
				writer.StartArray();
				val.serialize(*this);
				writer.EndArray();
			}
			else
			{
				val.serialize(*this);
			}
		}

		template<typename T, typename ... TT>
		void execute(MemberRef<T, TT...>&& mem)
		{
			writer.Key(mem.name.c_str());
			writer.StartObject();
			execute(mem.ref);
			writer.EndObject();
		}

		void execute(int val)
		{
			writer.Int(val);
		}

		void execute(float val)
		{
			writer.Double(val);
		}

		void execute(std::string val)
		{
			writer.String(val.c_str());
		}

	public:
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer;
	};

	template<typename ...TT>
	inline void SerializerSave::operator()(TT && ...args)
	{
		writer.StartObject();
		int ex[] = { this->ex(args)... };
		writer.EndObject();
	}

	template<typename T>
	inline void SerializerSave::member(const std::string & name, T & val)
	{
		writer.Key(name.c_str());
		execute(val);
	}
}