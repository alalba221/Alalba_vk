#include "pch.h"
#include "Log.h"

namespace Alalba
{
	std::shared_ptr<spdlog::logger> Log::s_Logger;
	
	void Log::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");
		s_Logger = spdlog::stdout_color_mt("ALALBA_VK");
		s_Logger->set_level(spdlog::level::trace);
	}
}