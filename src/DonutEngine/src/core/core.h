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
#endif