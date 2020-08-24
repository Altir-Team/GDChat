#pragma once
#include <functional>
#include <thread>
class Util
{
public:
	static void timer_start(std::function<void(void)> func, unsigned int interval)
	{
		std::thread([func, interval]() {
			while (true)
			{
				func();
				std::this_thread::sleep_for(std::chrono::milliseconds(interval));
			}
		}).detach();
	}
};

