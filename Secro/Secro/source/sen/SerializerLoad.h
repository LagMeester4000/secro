#pragma once
#include <string>
#include <type_traits>
#include <vector>
#include <memory>
#include "rapidjson/document.h"
#include "MemberRef.h"

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
			val.serialize(*this);
		}

		template<typename T, typename ... TT>
		void execute(MemberRef<T, TT...>&& mem)
		{
			if (!top().HasMember(mem.name.c_str()))
				return;

			push(top()[mem.name.c_str()]);
			execute(mem.ref);
			pop();
		}

		template<typename T>
		void execute(std::vector<T>&& val)
		{
			auto& topVal = top();
			if (!topVal.IsArray())
				return;

			for (size_t i = 0; i < topVal.Size(); ++i)
			{
				push(topVal[i]);
				T pushVal;
				execute(pushVal);
				val.push_back(std::move(pushVal));
				pop();
			}
		}

		template<typename T>
		void execute(std::shared_ptr<T>&& val)
		{
			if (std::is_base_of_v<Entity, T>)
			{

			}
		}

		void execute(int& val)
		{
			val = top().GetInt();
		}

		void execute(float& val)
		{
			val = top().GetDouble();
		}

		void execute(std::string& val)
		{
			val = top().GetString();
		}

		void pop();
		void push(rapidjson::Value& val);
		rapidjson::Value& top();

	public:
		rapidjson::Document document;
		std::vector<rapidjson::Value*> stack;
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