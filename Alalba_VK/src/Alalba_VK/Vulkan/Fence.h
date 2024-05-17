#pragma once
#include <vulkan/vulkan.h>
namespace vk
{
	class Device;
	
	class Fence
	{
	public:
		class Builder
		{
		public:
			Builder(const Device& device) :m_device(device){};
			Builder& SetTag(const std::string& tag) { m_tag = tag; return *this; }
			Builder& Signaled(bool signaled) { m_signaled = signaled; return *this; }
			std::unique_ptr<Fence> Build()const { return std::make_unique<Fence>(m_device,m_signaled,m_tag); };

		private:
			const Device& m_device;
			std::string m_tag{"Fence"};
			bool m_signaled;
		};

	public:
		VULKAN_NON_COPIABLE(Fence);
		Fence(const Device& device, const bool signaled, const std::string& tag );
		~Fence() { Clean(); };
		void Clean();
		void Reset();
		void Wait(uint64_t timeout) const;

	private:
		VULKAN_HANDLE(VkFence, m_fence);
		const class Device& m_device;

	};

	typedef std::unique_ptr<Fence> FencePtr;
}


