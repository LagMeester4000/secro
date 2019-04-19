#include "SerializerLoad.h"

using namespace sen;

void sen::SerializerLoad::pop()
{
	stack.pop_back();
}

void sen::SerializerLoad::push(rapidjson::Value & val)
{
	SerializeStackObj push;
	push.value = &val;
	stack.push_back(push);
}

void sen::SerializerLoad::push(SerializeStackObj & val)
{
	stack.push_back(val);
}

rapidjson::Value & sen::SerializerLoad::top()
{
	return *stack[stack.size() - 1].value;
}

SerializeStackObj & sen::SerializerLoad::topRaw()
{
	return stack[stack.size() - 1];
}

void sen::SerializerLoad::objectStart()
{
	auto& topVal = topRaw();
	if (topVal.isArray)
	{
		push((*topVal.value)[topVal.arrayIndex]);
		return;
	}
	//else
	push((*topVal.value)[topVal.memberFound.c_str()]);
}

void sen::SerializerLoad::objectEnd()
{
	pop();
	
	auto& topVal = topRaw();
	if (topVal.isArray)
		topVal.arrayIndex++;
}

void sen::SerializerLoad::arrayStart()
{
	auto& topVal = topRaw();
	SerializeStackObj pushVal;
	pushVal.value = &(*topVal.value)[topVal.memberFound.c_str()];
	pushVal.isArray = true;
	push(pushVal);
}

void sen::SerializerLoad::arrayEnd()
{
	pop();
}
