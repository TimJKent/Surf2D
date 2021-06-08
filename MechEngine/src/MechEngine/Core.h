#pragma once

#include <memory>

#ifdef ME_PLATFORM_WINDOWS
	#if ME_DYNAMIC_LINK
		#ifdef ME_BUILD_DLL
			#define MECHENGINE_API __declspec(dllexport)
		#else
			#define MECHENGINE_API __declspec(dllimport)
		#endif
	#else
		#define MECHENGINE_API
	#endif
#else
	#error MechEngine only supports Windows!
#endif

#ifdef ME_ENABLE_ASSERTS
	#define ME_ASSERT(x, ...){if(!(x)){ME_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();}}
	#define ME_CORE_ASSERT(x, ...){if(!(x)){ME_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();}}
#else
	#define ME_ASSERT(x, ...)
	#define ME_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1<<x)

#define ME_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)


namespace MechEngine {
	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T>
	using Ref = std::shared_ptr<T>;
}