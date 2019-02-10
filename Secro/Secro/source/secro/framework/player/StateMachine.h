#pragma once
#include <functional>
#include <array>
#include <map>
#include <vector>
#include "PlayerState.h"

namespace secro {
	class StateMachine {
		using SwitchFuncList = std::array<std::vector<std::function<void(float)>>, (size_t)PlayerState::MaxState>;
		using SwitchConditionList = std::map<PlayerState, std::map<PlayerState, std::function<bool(float)>>>;
	public:
		StateMachine();

		//update the state machine
		//this runs the switch condition checks for the current player state
		void update(float deltaTime, class PlayerCharacter* player);

		//add a condition for when the state machine will switch
		void addCondition(PlayerState from, PlayerState to, std::function<bool(float)> func);

		//add a function for when a state is set
		void addSetState(PlayerState to, std::function<void(float)> func);

		//add a function for when a state is unset
		void addUnsetState(PlayerState to, std::function<void(float)> func);

		//add a function for updating a state
		void addUpdateState(PlayerState state, std::function<void(float)> func);

		//forcefully change the state of the character
		void changeState(PlayerCharacter* player, PlayerState newState, float deltaTime);

		//call the unsetState methods, but the the setState methods
		void unsafeChangeState(PlayerCharacter* player, PlayerState newState, float deltaTime);

	private:
		//used to call functions when a state is set or unset
		//unset function is called first, then the set function is called
		SwitchFuncList setState;
		SwitchFuncList unsetState;

		//list of update functions for each state
		SwitchFuncList updateState;

		//the map with all the conditions for switching the state
		SwitchConditionList conditions;
	};
}