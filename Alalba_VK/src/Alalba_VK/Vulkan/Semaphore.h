#pragma once
#include <vulkan/vulkan.h>
namespace vk
{
	class Device;

	class Semaphore
	{
	public:
		class Builder
		{
		public:
			Builder(const Device& device) :m_device(device) {}
			Builder& SetTag(const std::string& tag) { m_tag = tag; return *this; }
			std::unique_ptr<Semaphore>Build() { return std::make_unique<Semaphore>(m_device, m_tag); };

		private:
			const Device& m_device;
			std::string m_tag{"Semaphore"};
		};

	public:
		VULKAN_NON_COPIABLE(Semaphore);
		Semaphore(const Device& device, const std::string& tag);
		~Semaphore() { Clean(); }
		void Clean();

	private:
		VULKAN_HANDLE(VkSemaphore, m_semaphore);
		const class Device& m_device;

	};
}


