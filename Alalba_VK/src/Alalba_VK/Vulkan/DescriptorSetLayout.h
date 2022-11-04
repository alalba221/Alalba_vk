
#pragma once
#include<vulkan/vulkan.h>
namespace vk
{
	class Device;
	class DescriptorSetLayout final
	{
  public:
    class Builder {
    public:
      Builder(const Device& device) : m_device{ device } {}

      Builder& SetTag(const std::string tag) { m_tag = tag; return *this; }
      // AddBinding: append a binding point to the layout
      Builder& AddBinding(
        uint32_t binding,
        VkDescriptorType descriptorType,
        VkShaderStageFlags stageFlags,
        uint32_t count = 1);
     std::unique_ptr<DescriptorSetLayout> Build() const;

    private:
      const class Device& m_device;
      std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_bindings{};
      std::string m_tag;
    };

    DescriptorSetLayout(const Device& device,
      std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings,
      const std::string& tag);

    ~DescriptorSetLayout() { Clean(); };

    VULKAN_NON_COPIABLE(DescriptorSetLayout);

    //const VkDescriptorSetLayout& GetVulkanDescriptorSetLayout() const { return m_descriptorSetLayout; }
    void Clean();
  
  private:
    VULKAN_HANDLE(VkDescriptorSetLayout, m_desLayout);
    const class Device& m_device;
   // VkDescriptorSetLayout m_descriptorSetLayout;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_bindings;

    //friend class VulkanDescriptorWriter;
	};
}

