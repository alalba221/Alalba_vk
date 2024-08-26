#include "pch.h"
#include "DescriptorSet.h"
#include "Device.h"
#include "DescriptorPool.h"
#include "DescriptorSetLayout.h"
#include "Buffer.h"
#include "Sampler.h"
#include "ImageView.h"
namespace vk
{
	DescriptorSet::Allocator& DescriptorSet::Allocator::SetDescSetLayout(const DescriptorSetLayout& descLayout)
	{
		m_descSetLayout = &descLayout;
		return *this;
	}
	
	DescriptorSet::DescriptorSet(const Device& device, const DescriptorPool& pool, 
		const DescriptorSetLayout* descLayout, 
		const std::string& tag)
		:m_device(device), m_descPool(pool),m_descLayout(descLayout), m_tag(tag)
	{
		LOG_INFO("Allocate Descriptor Set: {0}", m_tag);

		VkDescriptorSetLayout layout = m_descLayout->Handle();
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_descPool.Handle();
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &layout;

		VkResult err = vkAllocateDescriptorSets(m_device.Handle(), &allocInfo, &m_descriptorSet);
		ALALBA_ASSERT(err == VK_SUCCESS, "Create Descrtiptor Sets failed");

	}

	void DescriptorSet::Clean()
	{
		// LOG_WARN("Clean Descriptor Set: {0}", m_tag);
		//vkFreeDescriptorSets(
		//	m_device.Handle(),
		//	m_descPool.Handle(),
		//	1,
		//	&m_descriptorSet);
	}

	DescriptorSet& DescriptorSet::BindDescriptor(VkDescriptorType type, uint32_t binding, const Buffer& buffer, VkDeviceSize offset, VkDeviceSize range)
	{
		m_bufferDescInfo[binding] = { buffer.Handle(),offset,range };
		
		VkWriteDescriptorSet descriptorWrite = {};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = m_descriptorSet;
		descriptorWrite.dstBinding = binding;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = type;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &m_bufferDescInfo[binding];

		m_writes[binding] = descriptorWrite;
		return *this;
	}																																																							
	DescriptorSet& DescriptorSet::BindDescriptor(VkDescriptorType type, uint32_t binding, const Sampler& sampler, const ImageView& imageView, VkImageLayout imageLayout)
	{
		m_imageDescInfo[binding] = { sampler.Handle(), imageView.Handle(), imageLayout};

		VkWriteDescriptorSet descriptorWrite = {};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = m_descriptorSet;
		descriptorWrite.dstBinding = binding;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = type;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pImageInfo = &m_imageDescInfo[binding];

		m_writes[binding] = descriptorWrite;
		return *this;

	}
	void DescriptorSet::UpdateDescriptors()
	{
		std::vector<VkWriteDescriptorSet> writes;
		for (int i = 0; i < m_writes.size(); i++)
		{
			writes.push_back(m_writes[i]);
		}
		vkUpdateDescriptorSets(
			m_device.Handle(),
			writes.size(),
			writes.data(), 0, nullptr
		);
	}
}