#include "pch.h"
#include "Instance.h"
#include "Alalba_VK/Core/Window.h"

namespace vk
{
  static VkBool32 VkDebugReportCallback(VkDebugReportFlagsEXT flags,
    VkDebugReportObjectTypeEXT                  objectType,
    uint64_t                                    object,
    size_t                                      location,
    int32_t                                     messageCode,
    const char* pLayerPrefix,
    const char* pMessage,
    void* pUserData)
  {
    if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
    {
      LOG_ERROR("{0}", pMessage);
    }
    if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT || flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
      LOG_WARN("{0}", pMessage);
    }
    return VK_TRUE;
  }

  Instance::Builder& Instance::Builder::SetVulkanVersino(uint32_t version)
  {
    m_vulkanVersion = version;
    return *this;
  }

  Instance::Builder& Instance::Builder::RequestLayer(const char* layer)
  {
    m_requestLayers.push_back(layer);
    return *this;
  }

  std::unique_ptr<Instance> Instance::Builder::Build() const
  {
    return std::make_unique<Instance>(m_window, m_requestLayers, m_vulkanVersion);
  }


	Instance::Instance(const Alalba::Window* window, const std::vector<const char*>& requestLayers, uint32_t vulkanVersion )
    //:m_Layers(Layers)
	{
    //1. Layers
    uint32_t availableLayerCount = 0;
    CALL_VK(vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr));
    std::vector<VkLayerProperties> availableLayers(availableLayerCount);
    CALL_VK(vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data()));
    
    uint32_t enableLayerCount = 0;
    const char* enableLayers[32];
    if (!checkContextFeatures("Instance Layers", false, availableLayerCount, availableLayers.data(),
       requestLayers.size(), requestLayers, &enableLayerCount, enableLayers))
    {
        return;
    }

    //2. Extensions
    uint32_t availableExtensionCount;
    CALL_VK(vkEnumerateInstanceExtensionProperties("", &availableExtensionCount, nullptr));
    std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
    CALL_VK(vkEnumerateInstanceExtensionProperties("", &availableExtensionCount, availableExtensions.data()));

    auto requestedExtensions = window->GetRequiredInstanceExtensions();

#ifdef ALALBA_DEBUG    
    requestedExtensions.push_back("VK_EXT_debug_report");
//#else
//    createInfo.enabledLayerCount = 0;
#endif

    uint32_t enableExtensionCount;
    const char* enableExtensions[32];
    if (!checkContextFeatures("Instance Extension", true, availableExtensionCount, availableExtensions.data(),
      requestedExtensions.size(), requestedExtensions, &enableExtensionCount, enableExtensions))
    {
      return;
    }


    // 3. create instance
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.apiVersion = vulkanVersion;


    VkDebugReportCallbackCreateInfoEXT debugReportCallbackInfoExt{};

    debugReportCallbackInfoExt.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    debugReportCallbackInfoExt.pNext = nullptr;
    debugReportCallbackInfoExt.flags = VK_DEBUG_REPORT_WARNING_BIT_EXT
      | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT
      | VK_DEBUG_REPORT_ERROR_BIT_EXT;
    debugReportCallbackInfoExt.pfnCallback = VkDebugReportCallback;


    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
#ifdef ALALBA_DEBUG
    createInfo.pNext = &debugReportCallbackInfoExt;
#else
    createInfo.pNext = nullptr;
#endif
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledLayerCount = enableLayerCount;
    createInfo.ppEnabledLayerNames = enableLayerCount > 0 ? enableLayers : nullptr;
    createInfo.enabledExtensionCount = enableExtensionCount;
    createInfo.ppEnabledExtensionNames = enableExtensionCount > 0 ? enableExtensions : nullptr;

    VkResult err;
    err = vkCreateInstance(&createInfo, nullptr, &m_instance);
    ALALBA_ASSERT(err == VK_SUCCESS);
    LOG_TRACE("{0} : instance : {1}", __FUNCTION__, (void*)m_instance);
    
   

	}
  Instance::~Instance()
  {
    Clean();
  }

  void Instance::Clean()
  {
    if (m_instance != VK_NULL_HANDLE)
    {
      //LOG_WARN("Clean Vulkan Instance {0}", m_tag);
      vkDestroyInstance(m_instance, nullptr);
      m_instance = VK_NULL_HANDLE;
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
  void Instance::PrintPhyDeviceInfo(VkPhysicalDeviceProperties& props)
  {
    const char* deviceType = props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU ? "integrated gpu" :
      props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? "discrete gpu" :
      props.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU ? "virtual gpu" :
      props.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU ? "cpu" : "others";

    uint32_t driverVersionMajor = VK_VERSION_MAJOR(props.driverVersion);
    uint32_t driverVersionMinor = VK_VERSION_MINOR(props.driverVersion);
    uint32_t driverVersionPatch = VK_VERSION_PATCH(props.driverVersion);

    uint32_t apiVersionMajor = VK_VERSION_MAJOR(props.apiVersion);
    uint32_t apiVersionMinor = VK_VERSION_MINOR(props.apiVersion);
    uint32_t apiVersionPatch = VK_VERSION_PATCH(props.apiVersion);

    LOG_DEBUG("-----------------------------");
    LOG_DEBUG("deviceName       : {0}", props.deviceName);
    LOG_DEBUG("deviceType       : {0}", deviceType);
    LOG_DEBUG("vendorID         : {0}", props.vendorID);
    LOG_DEBUG("deviceID         : {0}", props.deviceID);
    LOG_DEBUG("driverVersion    : {0}.{1}.{2}", driverVersionMajor, driverVersionMinor, driverVersionPatch);
    LOG_DEBUG("apiVersion       : {0}.{1}.{2}", apiVersionMajor, apiVersionMinor, apiVersionPatch);
  }

  void Instance::SelectPhysicalDevice()
  {
    // Physics Device
    LOG_DEBUG("-----------------------------");
    LOG_DEBUG("Physical devices: ");
    GetPhysicalDevices();
    for (auto phyDevuce : m_physicalDevices)
    {
      VkPhysicalDeviceProperties props;
      vkGetPhysicalDeviceProperties(phyDevuce, &props);
      PrintPhyDeviceInfo(props);
    }
    // pick physic device
    m_pPhysicalDevice = PhysicalDevice::Selector(m_physicalDevices)
      .RequireExtention("VK_KHR_ray_tracing_pipeline")
      .RequireExtention("VK_KHR_swapchain")
      .RequireQueueFamily(VK_QUEUE_TRANSFER_BIT)
      .RequireQueueFamily(VK_QUEUE_COMPUTE_BIT | VK_QUEUE_GRAPHICS_BIT)
      .RequireGpuType(VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
      .Select();
  }
}