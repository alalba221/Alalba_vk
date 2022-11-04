#pragma once
#include <memory>

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
#include "spdlog/sinks/stdout_color_sinks.h"
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
// #define ALALBA_TRACE(...)		::Alalba_vk::Log::GetLogger()->trace(__VA_ARGS__)
// #define ALALBA_INFO(...)			::Alalba_vk::Log::GetLogger()->info(__VA_ARGS__)
// #define ALALBA_WARN(...)		::Alalba_vk::Log::GetLogger()->warn(__VA_ARGS__)
// #define ALALBA_ERROR(...)		::Alalba_vk::Log::GetLogger()->error(__VA_ARGS__)
// #define ALALBA_FATAL(...)		::Alalba_vk::Log::GetLogger()->fatal(__VA_ARGS__)

template<typename... Args>
inline void ALALBA_TRACE(const Args &... args)
{
	Alalba::Log::GetLogger()->trace(args...);
}
template<typename... Args>
inline void ALALBA_INFO(const Args &... args)
{
	Alalba::Log::GetLogger()->info(args...);
}
template<typename... Args>
inline void ALALBA_WARN(const Args &... args)
{
	Alalba::Log::GetLogger()->warn(args...);
}
template<typename... Args>
inline void ALALBA_ERROR(const Args &... args)
{
	Alalba::Log::GetLogger()->error(args...);
}
template<typename... Args>
inline void ALALBA_FATAL(const Args &... args)
{
	Alalba::Log::GetLogger()->critical(args...);
}
