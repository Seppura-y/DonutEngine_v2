#ifndef CORE_H
#define CORE_H

#ifdef DN_PLATFORM_WINDOWS
	#ifdef DN_BUILD_SHARED
		#define DONUT_API __declspec(dllexport)
	#else
		#define DONUT_API __declspec(dllimport)
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

#endif