#ifndef TIMER_H
#define TIMER_H

#include <chrono>

namespace Donut {

	class Timer
	{
	public:
		Timer()
		{
			reset();
		}

		void Timer::reset()
		{
			start_ = std::chrono::high_resolution_clock::now();
		}

		float Timer::elapsed()
		{
			return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start_).count() * 0.001f * 0.001f * 0.001f;
		}

		float Timer::elapsedMillis()
		{
			return elapsed() * 1000.0f;
		}

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> start_;
	};

}

#endif