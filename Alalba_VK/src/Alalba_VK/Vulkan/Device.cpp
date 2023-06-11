#include "pch.h"
#include "Device.h"
#include "PhysicalDevice.h"

#include"Alalba_VK/Core/Application.h"
namespace vk
{

	Device::Builder& Device::Builder::AddExtension(const char* ext)
	{
		m_exts.push_back(ext);
		return *this;
	}

	std::unique_ptr<Device> Device::Builder::Build()
	{
		return std::make_unique<Device>(m_physicalDevice, m_exts);
	}

	Device::Device(const PhysicalDevice& physicalDevice, const std::vector<const char*>& exts)
		:m_physicalDevice(physicalDevice)
	{
		ALALBA_INFO("Create Logical Device");
		// Do we need to enable any other extensions (eg. NV_RAYTRACING?)
		// If the device will be used for presenting to a display via a swapchain we need to request the swapchain extension
		std::vector<const char*> deviceExtensions;
		for (auto ext : exts)
		{
			ALALBA_ASSERT(m_physicalDevice.IsExtensionSupported(ext));
			deviceExtensions.push_back(ext);
		}

		// Required device features.
		VkPhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddressFeatures = {};
		bufferDeviceAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
		bufferDeviceAddressFeatures.pNext = nullptr;
		bufferDeviceAddressFeatures.bufferDeviceAddress = true;

		VkPhysicalDeviceDescriptorIndexingFeatures indexingFeatures = {};
		indexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
		indexingFeatures.pNext = &bufferDeviceAddressFeatures;
		indexingFeatures.runtimeDescriptorArray = true;
		indexingFeatures.shaderSampledImageArrayNonUniformIndexing = true;

		VkPhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeatures = {};
		accelerationStructureFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
		accelerationStructureFeatures.pNext = &indexingFeatures;
		accelerationStructureFeatures.accelerationStructure = true;

		VkPhysicalDeviceRayTracingPipelineFeaturesKHR rayTracingFeatures = {};
		rayTracingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
		rayTracingFeatures.pNext = &accelerationStructureFeatures;
		rayTracingFeatures.rayTracingPipeline = true;

		// config queues
		m_grapicsQ = Queue::Configurer()
			.SetQFamily(m_physicalDevice.GetQFamilies().graphics.value())
			.SetPriority(1.0f)
			.Configure();
		m_transferQ = Queue::Configurer()
			.SetQFamily(m_physicalDevice.GetQFamilies().transfer.value())
			.SetPriority(1.0f)
			.Configure();
		m_computeQ = Queue::Configurer()
			.SetQFamily(m_physicalDevice.GetQFamilies().compute.value())
			.SetPriority(1.0f)
			.Configure();


		// create info
		VkDeviceCreateInfo deviceCreateInfo{};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pQueueCreateInfos = Queue::s_qCreateInfos.data();
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(Queue::s_qCreateInfos.size());
		deviceCreateInfo.pEnabledFeatures = &(m_physicalDevice.GetPhysicalDeviceFeatures());
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
		deviceCreateInfo.pNext = &rayTracingFeatures;
		
		VkResult err;
		err = vkCreateDevice(m_physicalDevice.Handle(), &deviceCreateInfo, nullptr, &m_device);
		ALALBA_ASSERT(err == VK_SUCCESS, "Create Logical Device Failed");

		/// Other private members 
		vkGetDeviceQueue(this->Handle(), m_physicalDevice.GetQFamilies().graphics.value(), 0, &m_grapicsQ->m_queue);
		vkGetDeviceQueue(this->Handle(), m_physicalDevice.GetQFamilies().compute.value(), 0, &m_computeQ->m_queue);
		vkGetDeviceQueue(this->Handle(), m_physicalDevice.GetQFamilies().transfer.value(), 0, &m_transferQ->m_queue);
	}

	Device::~Device()
	{
		Clean();
	}

	void Device::Clean()
	{
		if (m_device != VK_NULL_HANDLE)
		{
			ALALBA_WARN("Clean Logical Device {0}", m_tag);
			vkDestroyDevice(m_device, nullptr);
			m_device = VK_NULL_HANDLE;
		}
	}

	void Device::WaitIdle() const
	{
		vkDeviceWaitIdle(m_device);
	}

	VkFormat Device::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const
	{
		for (VkFormat format : candidates)
		{
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(m_physicalDevice.Handle(), format, &props);
			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
			{
				return format;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) 
			{
				return format;
			}
		}

		ALALBA_ASSERT("failed to find supported format!")
	}
}