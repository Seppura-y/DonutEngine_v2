#ifndef CORE_H
#define CORE_H

#ifdef DN_PLATFORM_WINDOWS
	#ifdef DN_BUILD_STATIC
		#define DONUT_API
	#else
		#ifdef DN_BUILD_SHARED
			#define DONUT_API __declspec(dllexport)
		#else
			#define DONUT_API __declspec(dllimport)
		#endif
	#endif
#else
	#error Donut only support Windows!
#endif // DN_BUILD_DLL

#endif