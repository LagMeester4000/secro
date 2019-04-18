#include "SerializerLoad.h"

void sen::SerializerLoad::pop()
{
	stack.pop_back();
}

void sen::SerializerLoad::push(rapidjson::Value & val)
{
	stack.push_back(&val);
}

rapidjson::Value & sen::SerializerLoad::top()
{
	return *stack[stack.size() - 1];
}
