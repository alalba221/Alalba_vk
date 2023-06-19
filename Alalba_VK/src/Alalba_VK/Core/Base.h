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
/*
The first line declaresand deletes the copy constructor.It prevents creating new instances of the class by copying from an existing instance.

The second line declaresand deletes the move constructor.It prevents creating new instances of the class by moving from an existing instance.

The third line declaresand deletes the copy assignment operator. It prevents assigning the class from another instance.

The fourth line declaresand deletes the move assignment operator. It prevents moving the class from another instance.
*/
#define VULKAN_NON_COPIABLE(ClassName) \
	ClassName(const ClassName&) = delete; \
	ClassName(ClassName&&) = delete; \
	ClassName& operator = (const ClassName&) = delete; \
	ClassName& operator = (ClassName&&) = delete;

#define VULKAN_HANDLE(VulkanHandleType, name) \
public: \
	VulkanHandleType Handle() const { return name; } \
	const std::string Tag() const {return m_tag;}\
private: \
	VulkanHandleType name{};\
	std::string m_tag;


// Macro to get a procedure address based on a vulkan instance
#define GET_INSTANCE_PROC_ADDR(inst, entrypoint)                        \
{                                                                       \
	fp##entrypoint = reinterpret_cast<PFN_vk##entrypoint>(vkGetInstanceProcAddr(inst, "vk"#entrypoint)); \
	ALALBA_ASSERT(fp##entrypoint);                                     \
}

// Macro to get a procedure address based on a vulkan device
#define GET_DEVICE_PROC_ADDR(dev, entrypoint)                           \
{                                                                       \
	fp##entrypoint = reinterpret_cast<PFN_vk##entrypoint>(vkGetDeviceProcAddr(dev, "vk"#entrypoint));   \
	ALALBA_ASSERT(fp##entrypoint);                                     \
}