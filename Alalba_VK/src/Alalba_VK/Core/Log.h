#pragma once
#include <memory>
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include "spdlog/spdlog.h"

namespace Alalba
{
	class Log
	{
	public:
		static void Init();
		static std::shared_ptr<spdlog::logger>& GetLogger() { return s_Logger; };

	private:
		static std::shared_ptr<spdlog::logger> s_Logger;
	};
}
 #define LOG_TRACE(...)		SPDLOG_LOGGER_TRACE(Alalba::Log::GetLogger(),__VA_ARGS__)
 #define LOG_INFO(...)			SPDLOG_LOGGER_INFO(Alalba::Log::GetLogger(),__VA_ARGS__)
 #define LOG_WARN(...)			SPDLOG_LOGGER_WARN(Alalba::Log::GetLogger(),__VA_ARGS__)
 #define LOG_ERROR(...)		SPDLOG_LOGGER_ERROR(Alalba::Log::GetLogger(),__VA_ARGS__)
 #define LOG_DEBUG(...)		SPDLOG_LOGGER_DEBUG(Alalba::Log::GetLogger(),__VA_ARGS__)

