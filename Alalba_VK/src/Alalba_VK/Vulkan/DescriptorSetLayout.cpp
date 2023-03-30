#include "pch.h"
#include "DescriptorSetLayout.h"
#include "Device.h"
namespace vk 
{
  DescriptorSetLayout::Builder& DescriptorSetLayout::Builder::AddBinding(
    uint32_t binding,
    VkDescriptorType descriptorType,
    VkShaderStageFlags stageFlags,
    uint32_t count)
  {
    ALALBA_ASSERT(m_bindings.count(binding) == 0, "Binding already in use");
    VkDescriptorSetLayoutBinding layoutBinding{};
    layoutBinding.binding = binding;
    layoutBinding.descriptorType = descriptorType;
    layoutBinding.descriptorCount = count;
    layoutBinding.stageFlags = stageFlags;
    m_bindings[binding] = layoutBinding;
    return *this;
  }
  std::unique_ptr<DescriptorSetLayout> DescriptorSetLayout::Builder::Build() const {
    return std::make_unique<DescriptorSetLayout>(m_device, m_bindings, m_tag);
  }
  DescriptorSetLayout* DescriptorSetLayout::Builder::BuildRawPointer() const
  {
    return new DescriptorSetLayout(m_device, m_bindings, m_tag);
  }
  // *************** Descriptor Set Layout *********************
  DescriptorSetLayout::DescriptorSetLayout(
    const Device& device,
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings, 
    const std::string& tag)
    : m_device{ device }, m_bindings{ bindings },m_tag(tag)
  {
    ALALBA_INFO("Create DescriptorSet Layout: {0}", m_tag);
    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
    for (auto kv : m_bindings) {
      setLayoutBindings.push_back(kv.second);
    }

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
    descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
    descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

    VkResult err;
    err = vkCreateDescriptorSetLayout(
      m_device.Handle(),
      &descriptorSetLayoutInfo,
      nullptr,
      &m_desLayout);
    ALALBA_ASSERT(err == VK_SUCCESS, "failed to create descriptor set layout!");
  }

  void DescriptorSetLayout::Clean()
  {
    if (m_desLayout != VK_NULL_HANDLE)
    {
      ALALBA_WARN("Clean DescriptorSet Layout: {0}", m_tag);
      vkDestroyDescriptorSetLayout(m_device.Handle(), m_desLayout, nullptr);
      m_desLayout = VK_NULL_HANDLE;
    }
   
  }
}