#pragma once
#include <vulkan/vulkan.h>
namespace vk
{
	class Device;
	class ShaderModule final
	{
	public:
		class Builder
		{
		public:
			Builder(const Device& device) :m_device(device) {};
			Builder& SelectSpvFile(const std::string fileName) { m_fileName = fileName; return *this;};
			Builder& SetShaderStageBits(const VkShaderStageFlagBits& stageBits) { m_stageBits = stageBits; return *this; };
			Builder& SetTag(const std::string& tag) { m_tag = tag; return *this; }
			std::unique_ptr<ShaderModule> Build();

		private:
			const Device& m_device;
			std::string m_fileName{};
			VkShaderStageFlagBits m_stageBits{};
			std::string m_tag;
		};

	public:
		VULKAN_NON_COPIABLE(ShaderModule);
		ShaderModule(const Device& device, const std::string& filename, const VkShaderStageFlagBits& stageBits, const std::string& tag);
		const VkPipelineShaderStageCreateInfo& GetStageCI() const { return m_pipelineStageCI; }

		~ShaderModule() { Clean(); };
		void Clean();

	private:
		const std::vector<char> ReadSpvFile(const std::string& filename);

	private:
		VULKAN_HANDLE(VkShaderModule, m_shaderModule);

		const Device& m_device;
		VkPipelineShaderStageCreateInfo m_pipelineStageCI{};
	};
}

