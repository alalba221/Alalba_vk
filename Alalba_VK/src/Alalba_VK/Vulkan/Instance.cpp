#include "pch.h"
#include "Instance.h"
#include "Alalba_VK/Core/Window.h"

namespace vk
{
  Instance::Builder& Instance::Builder::SetVulkanVersino(uint32_t version)
  {
    m_vulkanVersion = version;
    return *this;
  }

  Instance::Builder& Instance::Builder::AddValidationLayer(const char* layer)
  {
    m_validationLayers.push_back(layer);
    return *this;
  }

  std::unique_ptr<Instance> Instance::Builder::Build() const
  {
    return std::make_unique<Instance>(m_window, m_validationLayers, m_vulkanVersion);
  }


	Instance::Instance(const Alalba::Window* window, const std::vector<const char*>& validationLayers, uint32_t vulkanVersion )
    :m_validationLayers(validationLayers)
	{
    ALALBA_INFO("Create Vulkan Instance");
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.apiVersion = vulkanVersion;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = window->GetRequiredInstanceExtensions();
    //ALALBA_ERROR("{0}", extensions[0]);
    /*std::vector<const char*>extensions;
    extensions.push_back("VK_KHR_win32_surface");
    extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);*/
#ifdef ALALBA_DEBUG    
    extensions.push_back("VK_EXT_debug_utils");
#else
    createInfo.enabledLayerCount = 0;
#endif

    createInfo.enabledLayerCount = static_cast<unsigned int>(validationLayers.size());;
    createInfo.ppEnabledLayerNames = validationLayers.data();
    createInfo.enabledExtensionCount = static_cast<unsigned int>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkResult err;
    err = vkCreateInstance(&createInfo, nullptr, &m_instance);
    ALALBA_ASSERT(err == VK_SUCCESS);

    // For other private data
    GetInstanceExtensions();
    GetInstanceLayers();
    GetPhysicalDevices();

    // pick physic device
    m_pPhysicalDevice = PhysicalDevice::Selector(m_physicalDevices)
      .RequireExtention("VK_KHR_ray_tracing_pipeline")
      .RequireExtention("VK_KHR_swapchain")
      .RequireQueueFamily(VK_QUEUE_TRANSFER_BIT)
      .RequireQueueFamily(VK_QUEUE_COMPUTE_BIT | VK_QUEUE_GRAPHICS_BIT)
      .RequireGpuType(VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
      .Select();

	}
  Instance::~Instance()
  {
    Clean();
  }

  void Instance::Clean()
  {
    if (m_instance != VK_NULL_HANDLE)
    {
      ALALBA_WARN("Clean Vulkan Instance {0}", m_tag);
      vkDestroyInstance(m_instance, nullptr);
      m_instance = VK_NULL_HANDLE;
    }
  }

  // Get extensions supported by the instance and store for later use
  void Instance::GetInstanceExtensions()
  {
    uint32_t extCount;
    vkEnumerateInstanceExtensionProperties(nullptr,&extCount, nullptr);
    ALALBA_ASSERT(extCount > 0);
    std::vector<VkExtensionProperties> extensions(extCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extCount, extensions.data());

    for (VkExtensionProperties extension : extensions)
    {
      m_supportedInstanceExtensions.push_back(extension.extensionName);
    }
  }
  void Instance::GetInstanceLayers()
  {
    uint32_t count;
    vkEnumerateInstanceLayerProperties(&count, nullptr);
    ALALBA_ASSERT(count > 0);
    m_layerProperties.resize(count);
    vkEnumerateInstanceLayerProperties(&count, m_layerProperties.data());

    for (auto layer : m_layerProperties)
    {
      ALALBA_ERROR("LAYER {0}", layer.layerName);
    }
  }
  void Instance::GetPhysicalDevices()
  {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
    ALALBA_ASSERT(deviceCount > 0, "No gpu found");
    m_physicalDevices.resize(deviceCount);
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, m_physicalDevices.data());
  }
}