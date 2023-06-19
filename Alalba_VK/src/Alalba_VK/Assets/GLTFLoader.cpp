#include "pch.h"
#include "GLTFLoader.h"
#include "Alalba_VK/Core/Application.h"
namespace Alalba
{
	GLTFLoader::GLTFLoader()
	{
		Application& app = Application::Get();
		m_allocator.reset(new vk::Allocator(app.GetDevice(), app.GetVulkanInstance(), "GLTFSys Allocator"));
		m_commandPool = vk::CommandPool::Builder(app.GetDevice())
			.SetTag("GLTFSys CmdPool")
			.SetFlags(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT)
			.SetQFamily(app.GetDevice().GetGraphicsQ().GetFamily())
			.Build();

		m_descPool = vk::DescriptorPool::Builder(app.GetDevice())
			.SetTag("gltf Descriptor Pool")
			.SetMaxSets(100)// Each material of every model requires a descriptor set
			//.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000)
			.Build();
	}

	GLTFLoader& GLTFLoader::LoadModel(const std::string& file)
	{
		ALALBA_ASSERT(m_glTFmodels.count(file) == 0, "gltfModel already exist");

		std::string modelName = file.substr(file.rfind("/") + 1, file.rfind(".") - file.rfind("/") - 1);
		m_glTFmodels.insert(std::make_pair(modelName, std::make_shared<GLTFModel>(*this, file)));
		return *this;
	}

	void GLTFLoader::Clean()
	{
		for (auto& models : m_glTFmodels)
		{
			models.second->Clean();
		}

		m_allocator->Clean();
		m_commandPool->Clean();
		m_descPool->Clean();
	}

}