#pragma once
#include<vulkan/vulkan.h>
namespace vk
{
	class Device;
	class DescriptorPool;
	class DescriptorSetLayout;
	class Buffer;
	class Sampler;
	class ImageView;

	class DescriptorSet
	{
	public:
		
		class Allocator
		{
		public:
			Allocator(const Device& device, const DescriptorPool& pool)
				:m_device(device), m_pool(pool) {}
			Allocator& SetTag(const std::string& tag) { m_tag = tag; return *this;}
			Allocator& SetDescSetLayout(const DescriptorSetLayout& descLayout);

			// the set = xx in the shader, xx is determined in function vkCmdBindDescriptorSets
			std::unique_ptr<DescriptorSet> Allocate()
			{
				return std::make_unique<DescriptorSet>(m_device, m_pool, m_descSetLayout, m_tag);
			}

		private:
			const class Device& m_device;
			const class DescriptorPool& m_pool;

			const DescriptorSetLayout* m_descSetLayout;
			uint32_t m_count = 1;

			std::string m_tag{ "Descriptor Set" };
		};

	public:
		VULKAN_NON_COPIABLE(DescriptorSet);
	
		DescriptorSet(const Device& device, const DescriptorPool& pool,
			const DescriptorSetLayout* descLayout,
			const std::string& tag);

		~DescriptorSet() { Clean(); };
		void Clean();

		// for descriptor
		DescriptorSet& BindDescriptor(VkDescriptorType type, uint32_t binding, const Buffer& buffer, VkDeviceSize offset, VkDeviceSize range);
		
		/** @brief 
		 *
		 *@ param imageLayout is the layout that the image subresources accessible from imageView will be in at the time this descriptor is accessed
		
		 */
		DescriptorSet& BindDescriptor(VkDescriptorType type, uint32_t binding, const Sampler& sampler, const ImageView& imageView, VkImageLayout imageLayout);
		//void Bind(const Image& image, VkDeviceSize offset, VkDeviceSize range);

		void UpdateDescriptors();


	private:
		VULKAN_HANDLE(VkDescriptorSet, m_descriptorSet);
		const class Device& m_device;
		const class DescriptorPool& m_descPool;
		
		const DescriptorSetLayout* m_descLayout;

		std::unordered_map<uint32_t, VkWriteDescriptorSet> m_writes;
		std::unordered_map<uint32_t, VkDescriptorBufferInfo> m_bufferDescInfo;
		std::unordered_map<uint32_t, VkDescriptorImageInfo> m_imageDescInfo;
	};
}
