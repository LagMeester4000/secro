#pragma once
#include <vector>
#include <string>
#include <memory>
#include "Serializer.h"
#include "Inspector.h"

namespace sen {
	using TypeIndex = size_t;

	template<typename Base>
	struct FactoryObject {
		std::string name;
		
		virtual ~FactoryObject() {}
		virtual bool isSameType(Base*) = 0;
		virtual Base* construct() = 0;
		virtual Base* copy(Base*) = 0;
		virtual void serializeSave(Base*, SerializerSave&) = 0;
		virtual void serializeLoad(Base*, SerializerLoad&) = 0;
		virtual void inspect(Base*, Inspector&) = 0;
	};

	template<typename Base, typename T>
	struct FactoryObjectImpl : FactoryObject<Base> {
		virtual bool isSameType(Base* base)
		{
			if (dynamic_cast<T*>(base))
			{
				return true;
			}
			return false;
		}
		virtual Base* construct()
		{
			return new T();
		}
		virtual Base* copy(Base* base)
		{
			return new T((T&)*base);
		}
		virtual void serializeSave(Base* obj, SerializerSave& serializer)
		{
			T* asT = (T*)obj;
			asT->serialize(serializer);
		}
		virtual void serializeLoad(Base* obj, SerializerLoad& serializer)
		{
			T* asT = (T*)obj;
			asT->serialize(serializer);
		}
		virtual void inspect(Base* obj, Inspector& inspector)
		{
			T* asT = (T*)obj;
			asT->serialize(inspector);
		}
	};

	template<typename Base>
	class Factory {
	public:
		static Factory<Base>& getInstance()
		{
			static Factory<Base> instance = Factory<Base>();
			return instance;
		}

		void registerType(std::shared_ptr<FactoryObject<Base>> type)
		{
			types.push_back(type);
		}

		FactoryObject<Base>& get(TypeIndex index)
		{
			return *types[index];
		}

		TypeIndex getType(Base* base)
		{
			for (size_t i = 0; i < types.size(); ++i)
			{
				auto& it = types[i];
				if (it->isSameType(base))
				{
					return (TypeIndex)i;
				}
			}
		}

		TypeIndex getType(std::string name)
		{
			for (size_t i = 0; i < types.size(); ++i)
			{
				auto& it = types[i];
				if (it->name == name)
				{
					return (TypeIndex)i;
				}
			}
		}

	private:
		std::vector<std::shared_ptr<FactoryObject<Base>>> types;
	};
}

#define REGISTER_COMPONENT_CPP(ClassName) \
int register_##ClassName() \
{ \
	sen::Factory<sen::Component>::registerType(std::make_shared<sen::FactoryObjImpl<ClassName>>(#ClassName)); \
	return 0; \
} \
\
int callRegister_##ClassName = register_##ClassName();

#define REGISTER_ENTITY_CPP(ClassName) \
int register_##ClassName() \
{ \
	auto reg = std::make_shared<sen::FactoryObjectImpl<sen::Entity, ClassName>>(); \
	reg->name = #ClassName; \
	sen::Factory<sen::Entity>::getInstance().registerType(reg); \
	return 0; \
} \
\
int callRegister_##ClassName = register_##ClassName();

#define REGISTER_LEVEL_CPP(ClassName) \
int register_##ClassName() \
{ \
	sen::Factory<sen::Level>::registerType(std::make_shared<sen::FactoryObjImpl<ClassName>>(#ClassName)); \
	return 0; \
} \
\
int callRegister_##ClassName = register_##ClassName();