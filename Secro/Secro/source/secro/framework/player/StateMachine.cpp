#include "StateMachine.h"
#include "PlayerCharacter.h"

#include <iostream>

secro::StateMachine::StateMachine()
{
}

void secro::StateMachine::update(float deltaTime, PlayerCharacter * player)
{
	auto state = player->getState();
	
	auto trans = conditions.find(state);
	if (trans == conditions.end())
	{
		std::cout << "no transition condition could be found for state " << (int)state << std::endl;
		return;
	}

	for (auto& it : trans->second)
	{
		if (it.second(deltaTime))
		{
			//call unset functions
			auto& unsetFuncs = unsetState[(int)state];
			for (auto& f : unsetFuncs)
				f(deltaTime);

			//call set functions
			auto setFuncs = setState[(int)it.first];
			for (auto& f : setFuncs)
				f(deltaTime);
			
			//set the actual player state
			player->state = it.first;
		}
	}
}

void secro::StateMachine::addCondition(PlayerState from, PlayerState to, std::function<bool(float)> func)
{
	conditions[from][to] = func;
}

void secro::StateMachine::addSetState(PlayerState to, std::function<void(float)> func)
{
	setState[(int)to].push_back(func);
}

void secro::StateMachine::addUnsetState(PlayerState to, std::function<void(float)> func)
{
	unsetState[(int)to].push_back(func);
}

void secro::StateMachine::changeState(PlayerCharacter * player, PlayerState newState, float deltaTime)
{
	auto oldState = player->getState();

	//call unset functions
	auto& unsetFuncs = unsetState[(int)oldState];
	for (auto& f : unsetFuncs)
		f(deltaTime);

	//call set functions
	auto setFuncs = setState[(int)newState];
	for (auto& f : setFuncs)
		f(deltaTime);

	//set the actual player state
	player->state = newState;
}

void secro::StateMachine::unsafeChangeState(PlayerCharacter * player, PlayerState newState, float deltaTime)
{
	auto oldState = player->getState();

	//call unset functions
	auto& unsetFuncs = unsetState[(int)oldState];
	for (auto& f : unsetFuncs)
		f(deltaTime);

	//set the actual player state
	player->state = newState;
}
