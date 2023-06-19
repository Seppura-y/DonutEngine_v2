#ifndef LOGGER_H
#define LOGGER_H

#include "core.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

#include <memory>

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

	std::shared_ptr<spdlog::logger>& Logger::getCoreLogger() { return sptr_core_logger_; }
	std::shared_ptr<spdlog::logger>& Logger::getClientLogger() { return sptr_client_logger_; }

	template<typename OStream, glm::length_t L, typename T, glm::qualifier Q>
	inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
	{
		return os << glm::to_string(vector);
	}

	template<typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
	inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
	{
		return os << glm::to_string(matrix);
	}

	template<typename OStream, typename T, glm::qualifier Q>
	inline OStream& operator<<(OStream& os, glm::qua<T, Q> quaternion)
	{
		return os << glm::to_string(quaternion);
	}
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