#pragma once
#include <string>
#include <type_traits>
#include <vector>
#include <memory>
#include "rapidjson/document.h"
#include "MemberRef.h"

namespace sen {
	struct SerializeStackObj {
		rapidjson::Value* value = nullptr;
		std::string memberFound = "";
		bool isArray = false;
		int arrayIndex = 0;
	};

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
				arrayStart();
				val.serialize(*this);
				arrayEnd();
			}
			else
			{
				val.serialize(*this);
			}
		}

		template<typename T, typename ... TT>
		void execute(MemberRef<T, TT...>&& mem)
		{
			if (!top().HasMember(mem.name.c_str()))
				return;

			//push(top()[mem.name.c_str()]);
			//execute(mem.ref);
			//pop();
			topRaw().memberFound = mem.name;
			execute(mem.ref);
			//top().Accept()
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
			auto& readVal = (*topRaw().value)[topRaw().memberFound.c_str()];
			val = readVal.GetInt();
		}

		void execute(float& val)
		{
			auto& readVal = (*topRaw().value)[topRaw().memberFound.c_str()];
			val = readVal.GetDouble();
		}

		void execute(std::string& val)
		{
			auto& readVal = (*topRaw().value)[topRaw().memberFound.c_str()];
			val = readVal.GetString();
		}

		void pop();
		void push(rapidjson::Value& val);
		void push(SerializeStackObj& val);
		rapidjson::Value& top();
		SerializeStackObj& topRaw();
		
		void objectStart();
		void objectEnd();
		void arrayStart();
		void arrayEnd();

	public:
		rapidjson::Document document;
		std::vector<SerializeStackObj> stack;
	};

	template<typename ...TT>
	inline void SerializerLoad::operator()(TT && ...args)
	{
		objectStart();
		int ex[] = { this->ex(args)... };
		objectEnd();
	}

	template<typename T>
	inline void SerializerLoad::member(const std::string & name, T & val)
	{
		topRaw().memberFound = name;
		execute(val);
	}
}