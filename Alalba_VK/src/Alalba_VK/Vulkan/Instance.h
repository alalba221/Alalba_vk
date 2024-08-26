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

		const std::vector<std::string>& Extensions() const { return m_supportedInstanceExtensions; }
		const std::vector<VkLayerProperties>& Layers() const { return m_layerProperties; }
		const std::vector<VkPhysicalDevice>& PhysicalDevices() const { return m_physicalDevices; }
		const std::vector<const char*>& ValidationLayers() const { return m_Layers; }
		
	private: // data
		VULKAN_HANDLE(VkInstance, m_instance);
		std::vector<std::string> m_supportedInstanceExtensions;

		std::vector<const char*> m_Layers;
		std::vector<VkLayerProperties> m_layerProperties;
		

	private: //function
		void GetPhysicalDevices();

		//********************************** physical device related*************************************************
	public:
		const PhysicalDevice& GetPhysicalDevice() const{ return *m_pPhysicalDevice; }
	private:
		std::vector<VkPhysicalDevice> m_physicalDevices;
		std::unique_ptr<PhysicalDevice> m_pPhysicalDevice;
	};
}
