#pragma once

namespace secro {
	class NetworkDebugUI {
	public:
		//update values in UI
		void update(float ping, float fps, float timeDiff, int frame);

		//render the debug windows
		void render();

	private:
		float pingOverTime[120];
		float fpsOverTime[120];
		float timeDiffOverTime[120];
		int arSize = 120;
		int current = 0;
		int currentFrame = 0;
	};
}