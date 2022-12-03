#pragma once
#include<vulkan/vulkan.h>

namespace vk
{
	class Device;

	class Sampler
	{
	public:

		class Builder
		{
		public:
			Builder(const Device& device) : m_device(device) {};
			Builder& SetTag(const std::string& tag) { m_tag = tag; return *this; }
			Builder& SetFilter(VkFilter filter) { m_filter = filter; return *this; }
			Builder& SetMipmapMode(VkSamplerMipmapMode mipmapMode) { m_mimpmapMode = m_mimpmapMode; return *this; }
			Builder& SetAddressMode(VkSamplerAddressMode addressMode) { m_addressMode = addressMode; return *this; }

			std::unique_ptr <Sampler> Build() const
			{
				return std::make_unique<Sampler>(m_device, m_filter, m_mimpmapMode, m_addressMode, m_tag);
			}

		private:
			const class Device& m_device;
			std::string m_tag;
			VkFilter m_filter;
			VkSamplerMipmapMode m_mimpmapMode;
			VkSamplerAddressMode m_addressMode;
		};

	public:
		VULKAN_NON_COPIABLE(Sampler);
		Sampler(const Device& device, VkFilter filter, VkSamplerMipmapMode mipmapMode,
				VkSamplerAddressMode addressMode, const std::string& tag);
		~Sampler() { Clean(); }
		void Clean();

	private:
		VULKAN_HANDLE(VkSampler, m_sampler);
		const class Device& m_device;

	};
}


