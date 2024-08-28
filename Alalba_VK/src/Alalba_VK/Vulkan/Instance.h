#pragma once
#include <vulkan/vulkan.h>
#include "PhysicalDevice.h"
#include "VkCommon.h"
namespace Alalba
{
	class Window ;
}

namespace vk {

	class Instance
	{
	public:
		class Builder
		{
		public:
			Builder(Alalba::Window* window) :m_window(window) {};
			Builder& SetVulkanVersino(uint32_t version);
			Builder& RequestLayer(const char* layer);
			std::unique_ptr<Instance> Build() const;
		private:
			Alalba::Window* m_window = nullptr;
			uint32_t m_vulkanVersion = 0;
			std::vector<const char*> m_requestLayers{};
		};

	public:
		VULKAN_NON_COPIABLE(Instance);
		Instance(const Alalba::Window* window, const std::vector<const char*>& validationLayers, uint32_t vulkanVersion);
		~Instance();
		void Clean();

		const std::vector<VkPhysicalDevice>& PhysicalDevices() const { return m_physicalDevices; }

		
	private: // data
		VULKAN_HANDLE(VkInstance, m_instance);

		

	private: //function
		void GetPhysicalDevices();

		//********************************** physical device related*************************************************
	public:
		const PhysicalDevice& GetPhysicalDevice() const{ return *m_pPhysicalDevice; }
		void PrintPhyDeviceInfo(VkPhysicalDeviceProperties& props);
		
		void SelectPhysicalDevice();

	private:
		std::vector<VkPhysicalDevice> m_physicalDevices;
		std::unique_ptr<PhysicalDevice> m_pPhysicalDevice;
	};
}
