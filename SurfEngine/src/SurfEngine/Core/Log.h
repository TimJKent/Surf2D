#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace SurfEngine {
	 class SurfEngine_API Log
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
#define SE_CORE_TRACE(...)	::SurfEngine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define SE_CORE_INFO(...)	::SurfEngine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define SE_CORE_WARN(...)	::SurfEngine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define SE_CORE_ERROR(...)	::SurfEngine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define SE_CORE_FATAL(...)	::SurfEngine::Log::GetCoreLogger()->fatal(__VA_ARGS__)

//Client log macros
#define SE_TRACE(...)	::SurfEngine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define SE_INFO(...)	::SurfEngine::Log::GetClientLogger()->info(__VA_ARGS__)
#define SE_WARN(...)	::SurfEngine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define SE_ERROR(...)	::SurfEngine::Log::GetClientLogger()->error(__VA_ARGS__)
#define SE_FATAL(...)	::SurfEngine::Log::GetClientLogger()->fatal(__VA_ARGS__)