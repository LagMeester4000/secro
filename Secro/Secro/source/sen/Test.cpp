#include "Test.h"
#include "Entity.h"
#include "Serializer.h"
#include "Factory.h"
#include "rapidjson/rapidjson.h"
#include <iostream>

class PlayerEntity : public sen::Entity {
public:
	int lives;
	float jumpHeight;

	template<typename T>
	void serialize(T& t)
	{
		sen::Entity::serialize(t);

		t(
			MEMBER(lives),
			MEMBER(jumpHeight)
		);
	}
};

REGISTER_ENTITY_CPP(PlayerEntity);
//int register_PlayerEntity() 
//{ 
//	sen::Factory<sen::Entity>::getInstance().registerType(std::make_shared<sen::FactoryObjectImpl<sen::Entity, PlayerEntity>>("PlayerEntity"));
//	return 0; 
//} 
//
//int callRegister_PlayerEntity = register_PlayerEntity();

void sen::test()
{
	SerializerSave ser;
	{
		std::shared_ptr<PlayerEntity> e = std::make_shared<PlayerEntity>();
		_initEntity(&*e, nullptr, "PlayerEntity", "piet", "");

		e->lives = 24;
		e->jumpHeight = 30.f;

		ser(MEMBER(e));
		std::cout << ser.buffer.GetString() << std::endl;
	}

	SerializerLoad serLoad;
	serLoad.parse(ser.buffer.GetString());
	{
		std::shared_ptr<PlayerEntity> e;
		serLoad(MEMBER(e));
		int look = 0;
	}
}
