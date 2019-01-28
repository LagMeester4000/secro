#pragma once
#include <array>
#include "../collision/FrameData.h"
#include "PlayerState.h"

namespace secro {
	//a collection of attack frame data
	class AttackCollection {
		constexpr static int size = (int)PlayerState::ALastAttack - (int)PlayerState::AFirstAttack - 1;
	public:
		AttackCollection();

		//load an attack in
		void loadAttack(std::string fileName, PlayerState attack);
		static FrameData loadRaw(std::string fileName);

		//get an attack from the attack list
		FrameData& getAttack(PlayerState attack);

	private:
		int findIndex(PlayerState state);

	private:
		std::array<FrameData, size> attacks;
	};
}