#pragma once
#include <functional>

namespace secro {
	struct Timer {
		std::function<void()> function;
		float duration;
		float timer = 0.f;
	};
	
	class TimerManager {
	public:
		TimerManager();

		//update all the timers 
		//call functions and remove if done
		void update(float deltaTime);

		//add a timer to the timer list
		void addTimer(std::function<void()> function, float duration);

	private:
		std::vector<Timer> timers;
	};
}