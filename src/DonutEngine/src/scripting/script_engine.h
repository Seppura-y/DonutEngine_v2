#ifndef SCRIPT_ENGINE_H
#define SCRIPT_ENGINE_H

namespace Donut
{
	class ScriptEngine
	{
	public:
		static void init();
		static void shutdown();
	private:
		static void initMono();
		static void shutdownMono();
	};
}

#endif