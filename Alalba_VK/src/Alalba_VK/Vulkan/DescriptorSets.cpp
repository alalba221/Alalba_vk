#include "pch.h"
#include "DescriptorSets.h"
#include "Device.h"
#include "DescriptorPool.h"
#include "DescriptorSetLayout.h"
#include "Buffer.h"
#include "Image.h"

namespace vk
{
	DescriptorSets::Allocator& DescriptorSets::Allocator::AddDescSetLayout(const DescriptorSetLayout& descLayout)
	{
		m_descSetLayouts.push_back(&descLayout);
		return *this;
	}
	
	DescriptorSets::Allocator& DescriptorSets::Allocator::Descriptor2Buffer(uint32_t index_of_set,uint32_t bind, const Buffer& buffer, VkDeviceSize offset, VkDeviceSize range)
	{
		ALALBA_ASSERT(m_descSetLayouts[index_of_set]->GetBindings().count(bind) == 1, "Binding not match with layout");
		
		m_descriptorInfo[index_of_set].push_back({bind,&buffer,offset,range});
	
		return *this;
	}

	// todo : 
	DescriptorSets::Allocator& DescriptorSets::Allocator::Descriptor2Image(uint32_t set, uint32_t bind, const Image& image, VkDeviceSize offset, VkDeviceSize range)
	{
		//ALALBA_ASSERT(m_descSetLayouts[set]->GetBindings().count(bind) == 1, "Binding not match with layout");
		//VkDescriptorImageInfo imageinfo{ image.Handle(),offset,range };
		//
		//return *this;
	}

	DescriptorSets::DescriptorSets(const Device& device, const DescriptorPool& pool, uint32_t count, 
		std::vector<const DescriptorSetLayout*> descLayouts, 
		const std::unordered_map<uint32_t, std::vector<DescriptorInfo>>& descriptorInfo,
		const std::string& tag)
		:m_device(device), m_descPool(pool), m_tag(tag)
	{
		ALALBA_INFO("Allocate Descriptor Set: {0}", m_tag);
		std::vector<VkDescriptorSetLayout> layouts;
		
		for (auto layout : descLayouts)
			layouts.push_back(layout->Handle());

		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_descPool.Handle();
		allocInfo.descriptorSetCount = count;
		allocInfo.pSetLayouts = layouts.data();

		m_descriptorSets.resize(count);

		VkResult err = vkAllocateDescriptorSets(m_device.Handle(), &allocInfo, m_descriptorSets.data());
		ALALBA_ASSERT(err == VK_SUCCESS, "Create Descrtiptor Sets failed");

		/// 
		for (int set = 0; set < descriptorInfo.size(); set++) // sets
		{	
			for (int descriptor = 0; descriptor < descriptorInfo.at(set).size();descriptor++)// descriptor
			{
				m_descriptorBufferInfo[set].push_back({ descriptorInfo.at(set)[descriptor].buffer->Handle(), descriptorInfo.at(set)[descriptor].offset, descriptorInfo.at(set)[descriptor].range});
				
				VkWriteDescriptorSet write{};
				write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				write.descriptorType = bindingDescription.descriptorType;
				write.dstBinding = binding;
				write.pBufferInfo = bufferInfo;
				write.descriptorCount = 1;
			}
		}
	
	}

	void DescriptorSets::Clean()
	{
		ALALBA_WARN("Clean Descriptor Set: {0}", m_tag);
		// You don't need to explicitly clean up descriptor sets, 
		// because they will be automatically freed when the descriptor pool is destroyed
		
		/*if (!m_descriptorSets.empty())
		{
			vkFreeDescriptorSets(
				m_device.Handle(),
				m_descPool.Handle(),
				static_cast<uint32_t>(m_descriptorSets.size()),
				m_descriptorSets.data());

			m_descriptorSets.clear();
		}*/
	}
}