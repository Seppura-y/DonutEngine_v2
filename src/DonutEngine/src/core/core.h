#ifndef CORE_H
#define CORE_H

#include "platform_detection.h"

#include <memory>

#if defined(DN_PLATFORM_WINDOWS)
#define DN_DEBUGBREAK() __debugbreak()
#elif defined(DN_PLATFORM_LINUX)
#include <signal.h>
#define DN_DEBUGBREAK() raise(SIGTRAP)
#else
#error "Platform doesn't support debugbreak yet!"
#endif

#ifdef DN_DEBUG
#define DN_ENABLE_ASSERTS
#endif

#ifndef DN_DIST
#define DN_ENABLE_VERIFY
#endif

#define DN_EXPAND_MACRO(x) x
#define DN_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)

//#define DN_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)
#define DN_BIND_EVENT_FN(fn) [this](auto&& ...args) ->decltype(auto) {return this->fn(std::forward<decltype(args)>(args)...);}


#ifdef DN_PLATFORM_WINDOWS
	#ifdef DN_BUILD_SHARED
		#ifdef DN_ENGINE
			#define DONUT_API __declspec(dllexport)
		#else
			#define DONUT_API __declspec(dllimport)
		#endif
	#else
		#define DONUT_API 
	#endif
#else
	#error Only support Windows!
#endif
//
//#define BIT(x) (1 << x)
//
//#ifdef DN_DEBUG
//#define DN_ENABLE_ASSERTS
//#endif

//#ifdef DN_ENABLE_ASSERTS
//#define DN_ASSERT(x, ...) { if(!(x)) { DN_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
//#define DN_CORE_ASSERT(x, ...) { if(!(x)) { DN_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
//#else
//#define DN_ASSERT(x, ...)
//#define DN_CORE_ASSERT(x, ...)
//#endif

#ifdef DN_ENABLE_ASSERTS

// Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
#define DN_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { DN##type##ERROR(msg, __VA_ARGS__); DN_DEBUGBREAK(); } }
#define DN_INTERNAL_ASSERT_WITH_MSG(type, check, ...) DN_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
#define DN_INTERNAL_ASSERT_NO_MSG(type, check) DN_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", DN_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)
		
#define DN_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define DN_INTERNAL_ASSERT_GET_MACRO(...) DN_EXPAND_MACRO( DN_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, DN_INTERNAL_ASSERT_WITH_MSG, DN_INTERNAL_ASSERT_NO_MSG) )

// Currently accepts at least the condition and one additional parameter (the message) being optional
#define DN_ASSERT(...) DN_EXPAND_MACRO( DN_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
#define DN_CORE_ASSERT(...) DN_EXPAND_MACRO( DN_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
#define DN_ASSERT(...)
#define DN_CORE_ASSERT(...)
#endif




#ifdef DN_ENABLE_VERIFY

// Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
#define DN_INTERNAL_VERIFY_IMPL(type, check, msg, ...) { if(!(check)) { DN##type##ERROR(msg, __VA_ARGS__); DN_DEBUGBREAK(); } }
#define DN_INTERNAL_VERIFY_WITH_MSG(type, check, ...) DN_INTERNAL_VERIFY_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
#define DN_INTERNAL_VERIFY_NO_MSG(type, check) DN_INTERNAL_VERIFY_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", DN_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

#define DN_INTERNAL_VERIFY_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define DN_INTERNAL_VERIFY_GET_MACRO(...) DN_EXPAND_MACRO( DN_INTERNAL_VERIFY_GET_MACRO_NAME(__VA_ARGS__, DN_INTERNAL_VERIFY_WITH_MSG, DN_INTERNAL_VERIFY_NO_MSG) )

// Currently accepts at least the condition and one additional parameter (the message) being optional
#define DN_VERIFY(...) DN_EXPAND_MACRO( DN_INTERNAL_VERIFY_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
#define DN_CORE_VERIFY(...) DN_EXPAND_MACRO( DN_INTERNAL_VERIFY_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
#define DN_VERIFY(...)
#define DN_CORE_VERIFY(...)
#endif






namespace Donut
{
	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ...Args>
	constexpr Scope<T> createScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ...Args>
	constexpr Ref<T> createRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}


#endif