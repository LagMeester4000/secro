#pragma once

namespace secro {
	struct GameInstance {
		bool isOnline = false;

		static GameInstance instance;
	};
}