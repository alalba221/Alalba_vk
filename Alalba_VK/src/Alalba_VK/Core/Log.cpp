#include "pch.h"
#include "Log.h"

namespace Alalba
{
	std::shared_ptr<spdlog::logger> Log::s_Logger{};
	
	void Log::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");
		s_Logger = spdlog::stdout_color_mt<spdlog::async_factory>("ALALBA_VK");
		s_Logger->set_level(spdlog::level::trace);
		//spdlog::set_pattern("%^[%T] [%P-%t] [%l!L] [%20s:%-4#] - %v%$");
	}
}