#ifndef LOGGER_H
#define LOGGER_H

#include <memory>
#include "core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Donut
{
	class DONUT_API Logger
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


// Core log macros
#define DN_CORE_TRACE(...)    ::Donut::Logger::getCoreLogger()->trace(__VA_ARGS__)
#define DN_CORE_INFO(...)     ::Donut::Logger::getCoreLogger()->info(__VA_ARGS__)
#define DN_CORE_WARN(...)     ::Donut::Logger::getCoreLogger()->warn(__VA_ARGS__)
#define DN_CORE_ERROR(...)    ::Donut::Logger::getCoreLogger()->error(__VA_ARGS__)
#define DN_CORE_CRITICAL(...) ::Donut::Logger::getCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define DN_CLIENT_TRACE(...)         ::Donut::Logger::getClientLogger()->trace(__VA_ARGS__)
#define DN_CLIENT_INFO(...)          ::Donut::Logger::getClientLogger()->info(__VA_ARGS__)
#define DN_CLIENT_WARN(...)          ::Donut::Logger::getClientLogger()->warn(__VA_ARGS__)
#define DN_CLIENT_ERROR(...)         ::Donut::Logger::getClientLogger()->error(__VA_ARGS__)
#define DN_CLIENT_CRITICAL(...)      ::Donut::Logger::getClientLogger()->critical(__VA_ARGS__)

#define DN_ENGINE_TRACE(...)    ::Donut::Logger::getCoreLogger()->trace(__VA_ARGS__)
#define DN_ENGINE_INFO(...)     ::Donut::Logger::getCoreLogger()->info(__VA_ARGS__)
#define DN_ENGINE_WARN(...)     ::Donut::Logger::getCoreLogger()->warn(__VA_ARGS__)
#define DN_ENGINE_ERROR(...)    ::Donut::Logger::getCoreLogger()->error(__VA_ARGS__)
#define DN_ENGINE_CRITICAL(...) ::Donut::Logger::getCoreLogger()->critical(__VA_ARGS__)


#endif