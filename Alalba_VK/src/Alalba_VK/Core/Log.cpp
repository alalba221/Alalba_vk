#include "pch.h"
#include "Log.h"

namespace Alalba
{
	std::shared_ptr<spdlog::logger> Log::s_Logger{};
	
	void Log::Init()
	{
		//spdlog::set_pattern("%^[%T] %n: %v%$");
		//spdlog::set_pattern("%^%H:%M:%S:%e [%P-%t] [%1!L] [%20s:%-4#] - %v%$");
		s_Logger = spdlog::stdout_color_mt<spdlog::async_factory>("ALALBA_VK");
		s_Logger->set_level(spdlog::level::trace);
		s_Logger->set_pattern("%^%H:%M:%S:%e [%P-%t] [%1!L] [%20s:%-4#] - %v%$");
		//spdlog::set_pattern("%^[%T] [%P-%t] [%l!L] [%20s:%-4#] - %v%$");
	}
}