#pragma once
#include <filesystem>
#ifdef ALALBA_ENABLE_ASSERTS

	// Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
	// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
	#define ALALBA_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { ALALBA##type##ERROR(msg, __VA_ARGS__); ALALBA_DEBUGBREAK(); } }
	#define ALALBA_INTERNAL_ASSERT_WITH_MSG(type, check, ...) ALALBA_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
	#define ALALBA_INTERNAL_ASSERT_NO_MSG(type, check) ALALBA_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", ALALBA_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

	#define ALALBA_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define ALALBA_INTERNAL_ASSERT_GET_MACRO(...) ALALBA_EXPAND_MACRO( ALALBA_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, ALALBA_INTERNAL_ASSERT_WITH_MSG, ALALBA_INTERNAL_ASSERT_NO_MSG) )

	// Currently accepts at least the condition and one additional parameter (the message) being optional
	#define ALALBA_ASSERT(...) ALALBA_EXPAND_MACRO( ALALBA_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
	#define ALALBA_CORE_ASSERT(...) ALALBA_EXPAND_MACRO( ALALBA_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
	#define ALALBA_ASSERT(...)
	#define ALALBA_CORE_ASSERT(...)
#endif