#ifndef ENGINE_LOG_H
#define ENGINE_LOG_H

#include <memory>
#include "core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Donut
{
	class DONUT_API EngineLog
	{
	public:
		static void init();
		inline static std::shared_ptr<spdlog::logger>& getCoreLogger();
		inline static std::shared_ptr<spdlog::logger>& getClientLogger();
	private:
		static std::shared_ptr<spdlog::logger> sptr_core_logger_;
		static std::shared_ptr<spdlog::logger> sptr_client_logger_;
	};
}


//// Core log macros
//#define DN_CORE_TRACE(...)    ::Donut::EngineLog::getCoreLogger()->trace(__VA_ARGS__)
//#define DN_CORE_INFO(...)     ::Donut::EngineLog::getCoreLogger()->info(__VA_ARGS__)
//#define DN_CORE_WARN(...)     ::Donut::EngineLog::getCoreLogger()->warn(__VA_ARGS__)
//#define DN_CORE_ERROR(...)    ::Donut::EngineLog::getCoreLogger()->error(__VA_ARGS__)
//#define DN_CORE_CRITICAL(...) ::Donut::EngineLog::getCoreLogger()->critical(__VA_ARGS__)
//
//// Client log macros
//#define DN_CLIENT_TRACE(...)         ::Donut::EngineLog::getClientLogger()->trace(__VA_ARGS__)
//#define DN_CLIENT_INFO(...)          ::Donut::EngineLog::getClientLogger()->info(__VA_ARGS__)
//#define DN_CLIENT_WARN(...)          ::Donut::EngineLog::getClientLogger()->warn(__VA_ARGS__)
//#define DN_CLIENT_ERROR(...)         ::Donut::EngineLog::getClientLogger()->error(__VA_ARGS__)
//#define DN_CLIENT_CRITICAL(...)      ::Donut::EngineLog::getClientLogger()->critical(__VA_ARGS__)

#define DN_ENGINE_TRACE(...)    ::Donut::EngineLog::getCoreLogger()->trace(__VA_ARGS__)
#define DN_ENGINE_INFO(...)     ::Donut::EngineLog::getCoreLogger()->info(__VA_ARGS__)
#define DN_ENGINE_WARN(...)     ::Donut::EngineLog::getCoreLogger()->warn(__VA_ARGS__)
#define DN_ENGINE_ERROR(...)    ::Donut::EngineLog::getCoreLogger()->error(__VA_ARGS__)
#define DN_ENGINE_CRITICAL(...) ::Donut::EngineLog::getCoreLogger()->critical(__VA_ARGS__)


#endif