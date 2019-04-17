#include "Test.h"
#include "Entity.h"
#include "Serializer.h"
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

void sen::test()
{
	PlayerEntity e;
	_initEntity(&e, nullptr, "piet", "");

	SerializerSave ser;
	ser(MEMBER(e));
	std::cout << ser.buffer.GetString() << std::endl;
}
