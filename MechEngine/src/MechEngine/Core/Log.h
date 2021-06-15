#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace MechEngine {
	 class MECHENGINE_API Log
	{
	public:
		 static void Init();

		 inline static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		 inline static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static Ref<spdlog::logger> s_CoreLogger;
		static Ref<spdlog::logger> s_ClientLogger;
	 };
}

//Core log macros
#define ME_CORE_TRACE(...)	::MechEngine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define ME_CORE_INFO(...)	::MechEngine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define ME_CORE_WARN(...)	::MechEngine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define ME_CORE_ERROR(...)	::MechEngine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define ME_CORE_FATAL(...)	::MechEngine::Log::GetCoreLogger()->fatal(__VA_ARGS__)

//Client log macros
#define ME_TRACE(...)	::MechEngine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define ME_INFO(...)	::MechEngine::Log::GetClientLogger()->info(__VA_ARGS__)
#define ME_WARN(...)	::MechEngine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define ME_ERROR(...)	::MechEngine::Log::GetClientLogger()->error(__VA_ARGS__)
#define ME_FATAL(...)	::MechEngine::Log::GetClientLogger()->fatal(__VA_ARGS__)