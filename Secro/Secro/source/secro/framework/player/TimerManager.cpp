#include "TimerManager.h"

secro::TimerManager::TimerManager()
{
}

void secro::TimerManager::update(float deltaTime)
{
	for (size_t i = 0; i < timers.size(); ++i)
	{
		auto& it = timers[i];
		it.timer += deltaTime;
		if (it.timer >= it.duration)
		{
			it.function();
			timers.erase(timers.begin() + i);
			--i;
		}
	}
}

void secro::TimerManager::addTimer(std::function<void()> function, float duration)
{
	Timer push;
	push.duration = duration;
	push.function = function;
	timers.push_back(push);
}
