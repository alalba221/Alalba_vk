#pragma once
#include <vulkan/vulkan.h>
#include "VkCommon.h"
#include "Alalba_VK/Core/GFX/Device.h"
#include "Alalba_VK/Vulkan/GFXContext.h"

// composition queue
#include "Queue.h"
namespace vk
{
	// create from 
	class PhysicalDevice;
	
	class Device final: public Alalba::Device
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

		const Queue& GetGraphicsQ() const { return *(m_grapicsQList[0]); }
		const Queue& GetComputeQ()	const { return *(m_computeQList[0]); }
		const Queue& GetTransferQ() const { return *(m_transferQList[0]); }

		const Queue& GetGraphicsQ(uint32_t index) const { return *(m_grapicsQList[index]); }
		const Queue& GetComputeQ(uint32_t index)	const { return *(m_computeQList[index]); }
		const Queue& GetTransferQ(uint32_t index) const { return *(m_transferQList[index]); }
		
		void WaitIdle() const;



		/* helper*/
		// find a proper format statisfing features from candidates
		VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;
		VkBool32 FormatIsFilterable(VkFormat format, VkImageTiling tiling) const;
	private:
		VULKAN_HANDLE(VkDevice, m_device);

		// For now, each Qfamlily has only one Q availabe!!!
		
		std::vector< std::unique_ptr<Queue> > m_grapicsQList;
		std::vector< std::unique_ptr<Queue> > m_transferQList;
		std::vector< std::unique_ptr<Queue> > m_computeQList;


		const class PhysicalDevice& m_physicalDevice;
	};
}
