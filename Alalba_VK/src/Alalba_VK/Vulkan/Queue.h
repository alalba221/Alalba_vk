#pragma once
#include <vulkan/vulkan.h>
#include "VkCommon.h"
#include "Fence.h"
#include "Semaphore.h"
#include "CommandBuffers.h"

#include "SwapChain.h"
namespace vk
{
	class Queue final
	{
	public:
		class Configurer
		{
		public:
			Configurer(){};
			Configurer& SetQFamily(const uint32_t queueFamily);
			Configurer& SetPriority(const float priority);
			std::unique_ptr<Queue> Configure();
		private:
			float m_priority = 0.0f;
			uint32_t m_qFamily = 0;
 		};
	public:
		static std::vector<VkDeviceQueueCreateInfo> s_qCreateInfos;
		VULKAN_NON_COPIABLE(Queue);

		Queue(const uint32_t qFamily, const float priority);

		void Submit(const CommandBuffers& cmdBuffers, int bufferIndex,  
			const std::vector<Semaphore>& waitOn, const std::vector<VkPipelineStageFlags> waitStages,
			const std::vector<Semaphore>& completedSignal, const Fence& completedFence) const;

		VkResult Present(const Semaphore& waitOn, const SwapChain& sawpChain, uint32_t frameIdx)const;

		~Queue() {};

		const float GetPriority() const { return m_priority; }
		const uint32_t GetFamily() const { return m_queueFamily; }
	private:
		VULKAN_HANDLE(VkQueue, m_queue);

		float m_priority;
		uint32_t m_queueFamily;
		VkDeviceQueueCreateInfo m_createInfo{};

		friend class Device;
	};
}

