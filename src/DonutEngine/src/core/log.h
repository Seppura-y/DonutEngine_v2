#ifndef LOG_H
#define LOG_H

#include <memory>
#include "core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Donut
{
	class DONUT_API Log
	{
	public:
		static void init();
		inline static std::shared_ptr<spdlog::logger>& getCoreLogger() { return sptr_core_logger_; }
		inline static std::shared_ptr<spdlog::logger>& getClientLogger() { return sptr_client_logger_; }
	private:
		static std::shared_ptr<spdlog::logger> sptr_core_logger_;
		static std::shared_ptr<spdlog::logger> sptr_client_logger_;
	};
}


// Core log macros
#define DN_CORE_TRACE(...)    ::Donut::Log::getCoreLogger()->trace(__VA_ARGS__)
#define DN_CORE_INFO(...)     ::Donut::Log::getCoreLogger()->info(__VA_ARGS__)
#define DN_CORE_WARN(...)     ::Donut::Log::getCoreLogger()->warn(__VA_ARGS__)
#define DN_CORE_ERROR(...)    ::Donut::Log::getCoreLogger()->error(__VA_ARGS__)
#define DN_CORE_CRITICAL(...) ::Donut::Log::getCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define DN_CLIENT_TRACE(...)         ::Donut::Log::getClientLogger()->trace(__VA_ARGS__)
#define DN_CLIENT_INFO(...)          ::Donut::Log::getClientLogger()->info(__VA_ARGS__)
#define DN_CLIENT_WARN(...)          ::Donut::Log::getClientLogger()->warn(__VA_ARGS__)
#define DN_CLIENT_ERROR(...)         ::Donut::Log::getClientLogger()->error(__VA_ARGS__)
#define DN_CLIENT_CRITICAL(...)      ::Donut::Log::getClientLogger()->critical(__VA_ARGS__)


#endif