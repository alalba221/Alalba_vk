#pragma once
#include <vulkan/vulkan.h>
// composition queue
#include "Queue.h"
#include "DescriptorSetLayout.h"

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

		const Queue& GetGraphicsQ() const { return *(m_grapicsQ.get()); }
		const Queue& GetComputeQ()	const { return *(m_computeQ.get()); }
		const Queue& GetTransferQ() const { return *(m_transferQ.get()); }
	
		const uint32_t GetSetLayouCount() const { return m_descSetLayouts.size(); }
		const DescriptorSetLayout& GetDescriptorSetLayout(uint32_t index) const;

		//const SwapChain& GetSwapChain() const { return *m_SwapChain.get(); }

		/* helper*/
		// find a proper format statisfing features from candidates
		VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;
	private:
		VULKAN_HANDLE(VkDevice, m_device);

		// For now, each Qfamlily has only one Q availabe!!!
		std::unique_ptr<Queue> m_grapicsQ;
		std::unique_ptr<Queue> m_transferQ;
		std::unique_ptr<Queue> m_computeQ;

		// Descriptor set layout
		std::vector<std::unique_ptr<DescriptorSetLayout>> m_descSetLayouts;

		const class PhysicalDevice& m_physicalDevice;
	};
}
