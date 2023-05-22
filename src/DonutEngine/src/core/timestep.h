#ifndef TIMESTEP_H
#define TIMESTEP_H

namespace Donut
{
	class Timestep
	{
	public:
		Timestep(float time = 0.0f)
			: time_(time)
		{

		}

		operator float() const { return time_; }

		float getSeconds() const { return time_; }
		float getMilliseconds() const { return time_ * 1000.0f; }
	private:
		float time_;
	};
}

#endif