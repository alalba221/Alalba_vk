#pragma once
#include<vulkan/vulkan.h>
namespace vk
{
	class Device;
	class DescriptorPool;
	class DescriptorSetLayout;
	class Buffer;
	class Image;
	

	class DescriptorSets
	{
	public:
		
		struct DescriptorInfo
		{
			uint32_t binding;
			const Buffer* buffer;
			VkDeviceSize offset;
			VkDeviceSize range;
		};

		class Allocator
		{
		public:
			Allocator(const Device& device, const DescriptorPool& pool)
				:m_device(device), m_pool(pool) {}
			Allocator& SetTag(const std::string& tag) { m_tag = tag; return *this;}

			Allocator& AddDescSetLayout(const DescriptorSetLayout& descLayout);
			Allocator& SetCount(uint32_t count) { m_count = count; return *this; }

			// this set is not the set = xx in the shader, this is just the index of the m_descriptorSets
			Allocator& Descriptor2Buffer(uint32_t index_of_set, uint32_t bind , const Buffer& buffer, VkDeviceSize offset, VkDeviceSize range);
			Allocator& Descriptor2Image(uint32_t set, uint32_t bind, const Image& image, VkDeviceSize offset, VkDeviceSize range);

			std::unique_ptr<DescriptorSets> Allocate()
			{
				return std::make_unique<DescriptorSets>(m_device, m_pool, m_count, m_descSetLayouts, m_descriptorInfo, m_tag);
			}

		private:
			const class Device& m_device;
			const class DescriptorPool& m_pool;
			uint32_t m_count = 1;

			std::string m_tag{ "Descriptor Set" };
			std::vector<const DescriptorSetLayout*> m_descSetLayouts;

			std::unordered_map<uint32_t, std::vector<DescriptorInfo>& > m_descriptorInfo;
			
		};

	public:
		VULKAN_NON_COPIABLE(DescriptorSets);
	
		DescriptorSets(const Device& device, const DescriptorPool& pool, uint32_t count, 
			std::vector<const DescriptorSetLayout*> descLayouts,
			const std::unordered_map<uint32_t, std::vector<DescriptorInfo>>& descriptorInfo,
			const std::string& tag);
		
		~DescriptorSets() { Clean(); };
		void Clean();
		VkDescriptorSet Handle(uint32_t index) const { return m_descriptorSets[index]; }
	
	private:

		std::vector<VkDescriptorSet> m_descriptorSets;
		std::unordered_map<uint32_t, std::vector<VkWriteDescriptorSet>> m_writes;
		std::unordered_map<uint32_t, std::vector<VkDescriptorBufferInfo>> m_descriptorBufferInfo;
		std::string m_tag;

		const class Device& m_device;
		const class DescriptorPool& m_descPool;
	};
}
