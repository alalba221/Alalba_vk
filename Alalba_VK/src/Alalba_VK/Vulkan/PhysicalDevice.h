#pragma once
#include<vulkan/vulkan.h>
namespace vk 
{
	class PhysicalDevice final
	{
	public:
		class Selector
		{
		public:
			Selector(const std::vector<VkPhysicalDevice>& devices) : m_devices(devices){};

			Selector& RequireGpuType(uint32_t gpuType); // properties
			Selector& SupportGeometryShader(bool support = true) { return *this; };
			Selector& RequireExtention(std::string ext);
			Selector& RequireQueueFamily(uint32_t qFamily);
			std::unique_ptr<PhysicalDevice>Select();
		private:
			std::vector<VkPhysicalDevice> m_devices{};
			uint32_t m_requestedQFamilies = 0;
			uint32_t m_gpuType = 0;
			std::vector<std::string> m_exts{};
		};


	public:
		VULKAN_NON_COPIABLE(PhysicalDevice);
		struct QueueFamilies
		{
			std::optional<uint32_t> graphics;
			std::optional<uint32_t> compute;
			std::optional<uint32_t> transfer;

			bool IsComplete() {
				return graphics.has_value() &&
					compute.has_value() &&
					transfer.has_value();
			}
		};
		// Leave for later use 
		struct MemoryHeapIndices{};
		struct MemoryTypeIndices{};

	public:
		PhysicalDevice(const std::vector<VkPhysicalDevice>& devices, 
			const uint32_t requestedQFamilies, const uint32_t gpuType, const std::vector<std::string>& exts);
		~PhysicalDevice();

		bool IsExtensionSupported(const char* extensionName) const;
		const VkPhysicalDeviceFeatures& GetPhysicalDeviceFeatures() const { return m_features; }
		const QueueFamilies& GetQFamilies() const { return m_queueFamilies; }

		// memory type = memoryTypeIndex is an index identifying a memory type from the memoryTypes 
		// array of the VkPhysicalDeviceMemoryProperties structure.
		const uint32_t FindRequiredMemoryType(const uint32_t bitFiledType, const VkMemoryPropertyFlags& typeFlag ) const;
	
	private:
		bool IsDeviceSuitable(const VkPhysicalDevice& device, const uint32_t requestedFamily, 
			const uint32_t gpuType, const std::vector<std::string>& exts);
		QueueFamilies FindRequestedQFamilies(const VkPhysicalDevice& device, const uint32_t requestedFamily);
	
	private:
		VULKAN_HANDLE(VkPhysicalDevice, m_device);

		QueueFamilies m_queueFamilies;
		std::unordered_set<std::string> m_supportedExtensions;

		VkPhysicalDeviceProperties m_properties;
		VkPhysicalDeviceFeatures m_features;

		friend class Instance;
	};
}

