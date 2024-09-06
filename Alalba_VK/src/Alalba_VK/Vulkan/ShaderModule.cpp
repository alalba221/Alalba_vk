#include "pch.h"
#include "ShaderModule.h"
#include "Device.h"

namespace vk
{
	std::unique_ptr<ShaderModule> ShaderModule::Builder::Build()
	{
		return std::make_unique<ShaderModule>(m_device, m_fileName, m_stageBits, m_tag);
	}

	ShaderModule::ShaderModule(const Device& device, const std::string& filename, const VkShaderStageFlagBits& stageBits, const std::string& tag)
		:m_device(device),m_tag(tag)
	{
		//LOG_INFO("Create Shader Module: {0}",m_tag);
		std::vector<char> shaderCode = ReadSpvFile(filename);

		// create info
		VkShaderModuleCreateInfo createinfo{};
		createinfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createinfo.pNext = nullptr;
		createinfo.codeSize = shaderCode.size();
		createinfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());;
		VkResult err = vkCreateShaderModule(m_device.Handle(), &createinfo, nullptr, &m_shaderModule);
		ALALBA_ASSERT(err == VK_SUCCESS,"Create ShaderModule Failed");

		// pipeline 
		m_pipelineStageCI.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		m_pipelineStageCI.pNext = nullptr;
		m_pipelineStageCI.flags = 0;
		m_pipelineStageCI.pSpecializationInfo = nullptr;
		m_pipelineStageCI.pName = "main";

		m_pipelineStageCI.stage = stageBits;
		m_pipelineStageCI.module = m_shaderModule;
	}

	void ShaderModule::Clean()
	{
		if (m_shaderModule != VK_NULL_HANDLE)
		{
			//LOG_WARN("Clean Shader Module {0}", m_tag);
			vkDestroyShaderModule(m_device.Handle(), m_shaderModule, nullptr);
			m_shaderModule = VK_NULL_HANDLE;
		}
	}
	const std::vector<char> ShaderModule::ReadSpvFile(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);
		ALALBA_ASSERT(file.is_open(), filename);

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();
		return buffer;
	}
}