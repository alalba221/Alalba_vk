#pragma once
#include <vulkan/vulkan.h>
#include "VkCommon.h"
namespace vk
{
	class Device;
	class DescriptorPool
	{
	public:
		class Builder 
		{
		public:
			Builder(const Device& device) :m_device(device) {};
			Builder& SetTag(const std::string tag) { m_tag = tag; return *this; }
			Builder& AddPoolSize(VkDescriptorType descriptorType, uint32_t count) 
			{ 
				m_poolSizes.push_back({ descriptorType, count }); 
				return *this;
			};

			Builder& SetPoolFlags(VkDescriptorPoolCreateFlags flags) { m_poolFlags = flags; return *this;};
			Builder& SetMaxSets(uint32_t count) { m_maxSets = count; return *this; };
			std::unique_ptr<DescriptorPool> Build()
			{
				return std::make_unique<DescriptorPool>(m_device, m_poolFlags, m_maxSets, m_poolSizes, m_tag);
			}

		private:
			const class Device& m_device;
			std::vector<VkDescriptorPoolSize> m_poolSizes{};
			uint32_t m_maxSets = 1000;
			VkDescriptorPoolCreateFlags m_poolFlags = 0;
			std::string m_tag;
		};

	public:
		VULKAN_NON_COPIABLE(DescriptorPool);
		DescriptorPool(const Device& device, 
			VkDescriptorPoolCreateFlags flags, uint32_t maxSets, const std::vector<VkDescriptorPoolSize>& poolSizes,
			const std::string& tag);
		~DescriptorPool() { Clean(); }
		void Clean();
	private:
		VULKAN_HANDLE(VkDescriptorPool, m_descriptorPool);
		VkDescriptorPoolCreateFlags m_poolFlags = 0;
		const class Device& m_device;
	};
	typedef std::unique_ptr<DescriptorPool> DescriptorPoolPtr;
}


