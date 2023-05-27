#ifndef CORE_H
#define CORE_H

#include <memory>

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

#define BIT(x) (1 << x)

#ifdef DN_DEBUG
#define DN_ENABLE_ASSERTS
#endif

#ifdef DN_ENABLE_ASSERTS
#define DN_ASSERT(x, ...) { if(!(x)) { DN_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define DN_CORE_ASSERT(x, ...) { if(!(x)) { DN_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
#define DN_ASSERT(x, ...)
#define DN_CORE_ASSERT(x, ...)
#endif

#define DN_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)


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