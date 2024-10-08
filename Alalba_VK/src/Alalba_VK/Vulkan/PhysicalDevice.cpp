#include "pch.h"
#include "PhysicalDevice.h"

namespace vk
{
	
	PhysicalDevice::Selector& PhysicalDevice::Selector::RequireGpuType(uint32_t gpuType)
	{
		m_gpuType = gpuType;
		return *this;
	}

	PhysicalDevice::Selector& PhysicalDevice::Selector::RequireExtention(std::string ext)
	{
		m_exts.push_back(ext);
		return *this;
	}

	PhysicalDevice::Selector& PhysicalDevice::Selector::RequireQueueFamily(uint32_t qFamily)
	{
		m_requestedQFamilies |= qFamily;
		return *this;
	}

	std::unique_ptr<PhysicalDevice> PhysicalDevice::Selector::Select()
	{
		return std::make_unique<PhysicalDevice>(m_devices, m_requestedQFamilies, m_gpuType, m_exts);
	}

	bool PhysicalDevice::IsExtensionSupported(const char* extensionName) const
	{
			return m_supportedExtensions.find(std::string(extensionName)) != m_supportedExtensions.end();
	}

	PhysicalDevice::PhysicalDevice(const std::vector<VkPhysicalDevice>& devices, 
		const uint32_t requestedFamily, const uint32_t gpuType, const std::vector<std::string>& exts)
	{
		//LOG_INFO("Select Physical Device");
		for (const VkPhysicalDevice& device : devices)
		{
			if (IsDeviceSuitable(device, requestedFamily, gpuType, exts))
			{
				m_device = device;
				break;
			}
		}
		ALALBA_ASSERT(m_device != VK_NULL_HANDLE, "failed to find a suitable GPU!");
		LOG_TRACE("{0} : physical device:{1}, graphic queue: {2} : {3}, compute queue: {4} : {5}, transfer queue: {6} : {7}", __FUNCTION__, m_properties.deviceName,
			m_queueFamilies.graphics.value(), m_queueFamilies.graphics_count.value(),
			m_queueFamilies.compute.value(), m_queueFamilies.compute_count.value(),
			m_queueFamilies.transfer.value(), m_queueFamilies.transfer_count.value());
	
	}

	PhysicalDevice::~PhysicalDevice()
	{
	}

	//const uint32_t PhysicalDevice::FindRequiredMemoryType(const uint32_t  bitFiledType, const VkMemoryPropertyFlags& typeFlag) const
	//{
	//	// 1. find the  memory type corresponding to the bitFiledType ( get by vkGetBufferMemoryRequirements)
	//	// 2. check if the memory type supports the properties we need
	//	VkPhysicalDeviceMemoryProperties properties;
	//	vkGetPhysicalDeviceMemoryProperties(m_device, &properties);

	//	for (uint32_t i = 0; i < properties.memoryTypeCount; i++)
	//	{
	//		if ((bitFiledType & (1 << i)) &&
	//			(properties.memoryTypes[i].propertyFlags & typeFlag) == typeFlag)
	//		{
	//			return i;
	//		}
	//	}
	//}

	bool PhysicalDevice::IsDeviceSuitable(const VkPhysicalDevice& device,
		const uint32_t requestedQFamilies, const uint32_t gpuType, const std::vector<std::string>& exts)
	{

		// Query queue families
		m_queueFamilies = FindRequestedQFamilies(device, requestedQFamilies);
	
		/// We want a device that supports the ray tracing extension.
		// Get all supported device exts
		uint32_t extCount = 0;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, nullptr);
		if (extCount > 0)
		{
			std::vector<VkExtensionProperties> extensions(extCount);
			if (vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, &extensions.front()) == VK_SUCCESS)
			{
				//LOG_TRACE("	Selected physical device has {0} extensions", extensions.size());
				for (const auto& extension : extensions)
				{
					m_supportedExtensions.emplace(extension.extensionName);
					//LOG_TRACE("  Device extension {0}", extension.extensionName);
				}
			}
		}

		for (auto ext : exts)
		{
			if (m_supportedExtensions.find(ext) == m_supportedExtensions.end())
				return false;
		}

		vkGetPhysicalDeviceProperties(device, &m_properties);

		// https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/enabling_buffer_device_address.html
		VkPhysicalDeviceBufferDeviceAddressFeatures deviceAddrFeature{};
		deviceAddrFeature.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
		m_features.pNext = &deviceAddrFeature;
		m_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		vkGetPhysicalDeviceFeatures2(device, &m_features);
		
		return m_properties.deviceType == gpuType && deviceAddrFeature.bufferDeviceAddress &&
			m_features.features.geometryShader && m_features.features.samplerAnisotropy && m_queueFamilies.IsComplete();
	}

	PhysicalDevice::QueueFamilies PhysicalDevice::FindRequestedQFamilies(const VkPhysicalDevice& device, const uint32_t requestedQFamilies)
	{
		QueueFamilies indices;
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
		LOG_DEBUG("queue family     : {0}", queueFamilyCount);
		// Dedicated queue for compute not with graphics
		// Try to find a queue family index that supports compute but not graphics
		if (requestedQFamilies & VK_QUEUE_COMPUTE_BIT)
		{
			for (uint32_t i = 0; i < queueFamilies.size(); i++)
			{
				auto& queueFamilyProperties = queueFamilies[i];
				if ((queueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT) && ((queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0))
				{
					indices.compute = i;
					indices.compute_count = queueFamilyProperties.queueCount;
					break;
				}
			}
		}
		// Dedicated queue for transfer not with graphics
		// Try to find a queue family index that supports transfer but not graphics and compute
		if (requestedQFamilies & VK_QUEUE_TRANSFER_BIT)
		{
			for (uint32_t i = 0; i < queueFamilies.size(); i++)
			{
				auto& queueFamilyProperties = queueFamilies[i];
				if ((queueFamilyProperties.queueFlags & VK_QUEUE_TRANSFER_BIT) && ((queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) && ((queueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT) == 0))
				{
					indices.transfer = i;
					indices.transfer_count = queueFamilyProperties.queueCount;
					break;
				}
			}
		}
		// For other queue types or if no separate compute queue is present, 
		// return the first one to support the requested flags
		for (uint32_t i = 0; i < queueFamilies.size(); i++)
		{
			if ((requestedQFamilies & VK_QUEUE_TRANSFER_BIT) && indices.transfer == -1)
			{
				if (queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
				{
					indices.transfer = i;
					indices.transfer_count = queueFamilies[i].queueCount;
				}
			}

			if ((requestedQFamilies & VK_QUEUE_COMPUTE_BIT) && indices.compute == -1)
			{
				if (queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
				{
					indices.compute = i;
					indices.compute_count = queueFamilies[i].queueCount;
				}
			}
			//	// present queue
			//VkBool32 bSupportSurface;
			//vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface.Handle(), &bSupportSurface);
			//if (bSupportSurface && indices.compute.has_value() && indices.transfer.has_value()
			//	&& indices.compute.value()!=i && indices.transfer.value()!=i)
			//{
			//	indices.present = i;
			//	indices.present_count = queueFamilies[i].queueCount;
			//}

			if (requestedQFamilies & VK_QUEUE_GRAPHICS_BIT)
			{
				//if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT && indices.present.has_value() && indices.present.value()!=i)
				if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				{
					indices.graphics = i;
					indices.graphics_count = queueFamilies[i].queueCount;
				}
			}

		}		
		return indices;
	}
	
}