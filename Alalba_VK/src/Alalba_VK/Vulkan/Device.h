#pragma once
#include <vulkan/vulkan.h>
// composition queue
#include "Queue.h"

namespace vk
{
	// create from 
	class PhysicalDevice;
	
	class Device final
	{
	public:
		class Builder
		{
		public:
			Builder(const PhysicalDevice& physicalDevice) :m_physicalDevice(physicalDevice) {};
			Builder& AddExtension(const char* ext);
			std::unique_ptr<Device>Build();
		
		private:
			const class PhysicalDevice& m_physicalDevice;
			std::vector<const char*> m_exts{};
		};

	public:
		VULKAN_NON_COPIABLE(Device);
		Device(const PhysicalDevice& physicalDevice, const std::vector<const char*>& exts);
		~Device();
		void Clean();

		const Queue& GetGraphicsQ() const { return *(m_grapicsQ); }
		const Queue& GetComputeQ()	const { return *(m_computeQ); }
		const Queue& GetTransferQ() const { return *(m_transferQ); }
		
		void WaitIdle() const;

		//const DescriptorSetLayout& GetDescriptorSetLayout(uint32_t index) const;

		//const SwapChain& GetSwapChain() const { return *m_SwapChain; }

		/* helper*/
		// find a proper format statisfing features from candidates
		VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;
	private:
		VULKAN_HANDLE(VkDevice, m_device);

		// For now, each Qfamlily has only one Q availabe!!!
		std::unique_ptr<Queue> m_grapicsQ;
		std::unique_ptr<Queue> m_transferQ;
		std::unique_ptr<Queue> m_computeQ;
		const class PhysicalDevice& m_physicalDevice;
	};
}
