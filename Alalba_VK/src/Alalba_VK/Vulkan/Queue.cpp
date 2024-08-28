#include "pch.h"
#include "Queue.h"
#include "Device.h"
namespace vk
{
	
	Queue::Queue(const Device& device, const uint32_t qFamily, const uint32_t index, const float priority)
		:m_device(device),m_queueFamily(qFamily), m_index(index), m_priority(priority)
	{
		vkGetDeviceQueue(m_device.Handle(), m_queueFamily, m_index, &m_queue);
		LOG_TRACE("Create a new queue: Family:{0} - Index:{1} - {2}", m_queueFamily, m_index, (void*)m_queue);
	}

	void Queue::Submit(const CommandBuffers& cmdBuffers, int bufferIndex,
		const std::vector<Semaphore>& waitOn, const std::vector<VkPipelineStageFlags> waitStages,
		const std::vector<Semaphore>& completedSignal, const Fence& completedFence)const
	{
		VkCommandBuffer commandBuffers[]{ cmdBuffers[bufferIndex] };
		
		std::vector<VkSemaphore> waitSemaphores;
		for (auto& wait : waitOn)
		{
			waitSemaphores.push_back(wait.Handle());
		}
		
		std::vector<VkSemaphore> signalSemaphores;
		for (auto& comp : completedSignal)
		{
			signalSemaphores.push_back(comp.Handle());
		}

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = waitSemaphores.size();
		submitInfo.pWaitSemaphores = waitSemaphores.data();
		submitInfo.pWaitDstStageMask = waitStages.data();
		submitInfo.signalSemaphoreCount = signalSemaphores.size();
		submitInfo.pSignalSemaphores = signalSemaphores.data();
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

	void Queue::WaitIdle() const
	{
		CALL_VK(vkQueueWaitIdle(m_queue));
	}
	
}