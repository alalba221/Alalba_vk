#include "pch.h"
#include "Queue.h"
namespace vk
{
	std::vector<VkDeviceQueueCreateInfo> Queue::s_qCreateInfos{};

	Queue::Configurer& Queue::Configurer::SetQFamily(const uint32_t queueFamily)
	{
		m_qFamily = queueFamily;
		return *this;
	}

	Queue::Configurer& Queue::Configurer::SetPriority(const float priority)
	{
		m_priority = priority;
		return *this;
	}

	std::unique_ptr<Queue> Queue::Configurer::Configure()
	{
		return std::make_unique<Queue>(m_qFamily, m_priority);
	}

	Queue::Queue(const uint32_t qFamily, const float priority)
		:m_queueFamily(qFamily),m_priority(priority)
	{
		m_createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		m_createInfo.queueFamilyIndex = m_queueFamily;
		m_createInfo.queueCount = 1;
		m_createInfo.pQueuePriorities = &m_priority;

		s_qCreateInfos.push_back(m_createInfo);
	}

	void Queue::Submit(const CommandBuffers& cmdBuffers, int bufferIndex,
		const Semaphore& waitOn, VkPipelineStageFlags waitStage,
		const Semaphore& completedSignal, const Fence& completedFence)const
	{
		VkCommandBuffer commandBuffers[]{ cmdBuffers[bufferIndex] };
		VkSemaphore waitSemaphores[] = { waitOn.Handle() };
		VkSemaphore signalSemaphores[] = { completedSignal.Handle() };
		VkPipelineStageFlags waitStages[] = { waitStage };


		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = commandBuffers;
		VkResult err;
		err = vkQueueSubmit(this->Handle(),
			1, &submitInfo, completedFence.Handle());
		ALALBA_ASSERT(err == VK_SUCCESS, "Q submit failed");
	}

	VkResult Queue::Present(const Semaphore& waitOn, const SwapChain& swapChain, uint32_t frameIdx) const
	{
		VkSemaphore signalSemaphores[] = { waitOn.Handle()};
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { swapChain.Handle() };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &frameIdx;
		presentInfo.pResults = nullptr; // Optional

		return vkQueuePresentKHR(this->Handle(), &presentInfo);
	}
	
}