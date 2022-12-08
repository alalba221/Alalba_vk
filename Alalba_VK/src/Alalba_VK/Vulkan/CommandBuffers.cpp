#include "pch.h"
#include "CommandBuffers.h"
#include "Device.h"
#include "CommandPool.h"
namespace vk
{
	CommandBuffers::CommandBuffers(const Device& device, const CommandPool& cmdPool, const uint32_t count, bool oneTimeSubmit, const std::string& tag)
		:m_device(device),m_cmdPool(cmdPool), m_oneTimeSubmit(oneTimeSubmit),m_tag(tag)
	{
		ALALBA_INFO("Allocate Command Buffers: {0}, size: {1}", m_tag, count);
		VkCommandBufferAllocateInfo AI{};
		AI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		AI.pNext = nullptr;
		AI.commandPool = m_cmdPool.Handle();
		AI.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		AI.commandBufferCount = count;

		m_cmdBuffers.resize(count);
		VkResult err = vkAllocateCommandBuffers(m_device.Handle(), &AI, m_cmdBuffers.data());
		ALALBA_ASSERT(err == VK_SUCCESS, "Allocate Command Buffer Failed");
	}
	void CommandBuffers::BeginRecording(uint32_t index)
	{
		//ALALBA_INFO("Record Command Buffers: {0}, {1}", m_tag, index);
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = m_oneTimeSubmit? VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT:VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		beginInfo.pInheritanceInfo = nullptr; // Optional

		VkResult err = vkBeginCommandBuffer(m_cmdBuffers[index], &beginInfo);
		ALALBA_ASSERT(err == VK_SUCCESS, "Record command buffer failed");
	}

	
	void CommandBuffers::Flush(uint32_t index, const Queue& q)
	{
		const uint64_t DEFAULT_FENCE_TIMEOUT = 100000000000;
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_cmdBuffers[index];

		// Create fence to ensure that the command buffer has finished executing
		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = 0;
		VkFence fence;
		vkCreateFence(m_device.Handle(), &fenceCreateInfo, nullptr, &fence);
		// Submit to the queue
		vkQueueSubmit(q.Handle(), 1, &submitInfo, fence);
		// Wait for the fence to signal that command buffer has finished executing
		vkWaitForFences(m_device.Handle(), 1, &fence, VK_TRUE, DEFAULT_FENCE_TIMEOUT);

		vkDestroyFence(m_device.Handle(), fence, nullptr);
		Clean();
	}

	void CommandBuffers::EndRecording(uint32_t index)
	{
		vkEndCommandBuffer(m_cmdBuffers[index]);
	}
	void CommandBuffers::Clean()
	{
		if (!m_cmdBuffers.empty())
		{
			ALALBA_WARN("Clean Command Buffers {0}", m_tag);
			vkFreeCommandBuffers(m_device.Handle(), m_cmdPool.Handle(), static_cast<uint32_t>(m_cmdBuffers.size()), m_cmdBuffers.data());
			m_cmdBuffers.clear();
		}
	}
}