#include "pch.h"
#include "Device.h"
#include "PhysicalDevice.h"
#include "Queue.h"
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

		PhysicalDevice::QueueFamilies QInfos = m_physicalDevice.GetQFamilies();
		
		std::vector<float> GFXQPriorities(QInfos.graphics_count.value(),1.0f);
		std::vector<float> CompQPriorities(QInfos.compute_count.value(), 1.0f);
		std::vector<float> TransQPriorities(QInfos.transfer_count.value(), 1.0f);

		std::vector<VkDeviceQueueCreateInfo> QCIs;
		VkDeviceQueueCreateInfo GfxQCI{};
		GfxQCI.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		GfxQCI.queueFamilyIndex = QInfos.graphics.value();
		GfxQCI.queueCount = QInfos.graphics_count.value();
		GfxQCI.pQueuePriorities = GFXQPriorities.data();

		VkDeviceQueueCreateInfo CompQCI{};
		CompQCI.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		CompQCI.queueFamilyIndex = QInfos.compute.value();
		CompQCI.queueCount = QInfos.compute_count.value();
		CompQCI.pQueuePriorities = CompQPriorities.data();

		VkDeviceQueueCreateInfo TransQCI{};
		TransQCI.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		TransQCI.queueFamilyIndex = QInfos.transfer.value();
		TransQCI.queueCount = QInfos.transfer_count.value();
		TransQCI.pQueuePriorities = TransQPriorities.data();

		QCIs.push_back(GfxQCI); QCIs.push_back(CompQCI); QCIs.push_back(TransQCI);

		// create info
		VkDeviceCreateInfo deviceCreateInfo{};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pQueueCreateInfos = QCIs.data();
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(QCIs.size());
		deviceCreateInfo.enabledLayerCount = 0;
		deviceCreateInfo.ppEnabledLayerNames = nullptr;
		deviceCreateInfo.pEnabledFeatures = &(m_physicalDevice.GetPhysicalDeviceFeatures());
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
		deviceCreateInfo.pNext = &rayTracingFeatures;
		
		VkResult err;
		err = vkCreateDevice(m_physicalDevice.Handle(), &deviceCreateInfo, nullptr, &m_device);
		ALALBA_ASSERT(err == VK_SUCCESS, "Create Logical Device Failed");

		LOG_TRACE("Vulkan Device : {0}", (void*)m_device);

		/// Create Qs
		//m_grapicsQList.resize(QInfos.graphics_count.value());
		for (uint32_t i = 0; i < QInfos.graphics_count.value(); i++)
		{
			// m_grapicsQList[i].reset(new Queue(*this, QInfos.graphics.value(), i, GFXQPriorities[i]));
			m_grapicsQList.push_back(std::make_unique<Queue>(*this, QInfos.graphics.value(), i, GFXQPriorities[i]));
		}
		
		//m_computeQList.resize(QInfos.compute_count.value());
		for (uint32_t i = 0; i < QInfos.compute_count.value(); i++)
		{
			m_computeQList.push_back(std::make_unique<Queue>(*this, QInfos.compute.value(), i, CompQPriorities[i]));
		}

		//m_transferQList.resize(QInfos.transfer_count.value());
		for (uint32_t i = 0; i < QInfos.transfer_count.value(); i++)
		{
			m_transferQList.push_back(std::make_unique<Queue>(*this, QInfos.transfer.value(), i, TransQPriorities[i]));
		}
	}

	Device::~Device()
	{
		Clean();
	}

	void Device::Clean()
	{
		if (m_device != VK_NULL_HANDLE)
		{
			/*LOG_WARN("Clean Logical Device {0}", m_tag);*/
			vkDeviceWaitIdle(m_device);
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

	// Returns if a given format support LINEAR filtering
	VkBool32 Device::FormatIsFilterable(VkFormat format, VkImageTiling tiling) const
	{
		VkFormatProperties formatProps;
		vkGetPhysicalDeviceFormatProperties(m_physicalDevice.Handle(), format, &formatProps);

		if (tiling == VK_IMAGE_TILING_OPTIMAL)
			return formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;

		if (tiling == VK_IMAGE_TILING_LINEAR)
			return formatProps.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;

		//LOG_ERROR("Current format is {0} not support linear filtering", format);
		return false;
	}
}