#pragma once
#include<memory>
#ifdef ALALBA_DEBUG
	#define ALALBA_DEBUGBREAK() __debugbreak()
	#define ALALBA_ENABLE_ASSERTS
#else
	#define HZ_DEBUGBREAK()
#endif

#define ALALBA_EXPAND_MACRO(x) x
#define ALALBA_STRINGIFY_MACRO(x) #x

template<typename T>
using Scope = std::unique_ptr<T>;

template<typename T>
using Ref = std::shared_ptr<T>;

#include"Assert.h"

#define BIT(x) (1<<x)
#define ALALBA_BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

#define ARRAY_SIZE(r) (sizeof(r)/sizeof(r[0]))
