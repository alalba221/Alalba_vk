#include "pch.h"
#include "ObjModel.h"

#include "Alalba_VK/Core/Application.h"
namespace Alalba
{
	vk::DescriptorSetLayout* ObjModel::s_descLayout= nullptr;

	ObjModel::ObjModel(const Mesh& mesh, const Texture& texture, const Scene& scene)
		:m_mesh(mesh),m_texture(texture)
	{
		if(s_descLayout == nullptr)
		s_descLayout = vk::DescriptorSetLayout::Builder(Application::Get().GetDevice())
			.AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
			.SetTag("ObjModel DescSet Layout")
			.BuildRawPointer();

		Application& app = Application::Get();

		m_descSet = vk::DescriptorSet::Allocator(app.GetDevice(), scene.GetDescPool())
			.SetTag("Module Descritor Set")
			.SetDescSetLayout(*s_descLayout)
			.Allocate();
	
		m_descSet->BindDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0,
			texture.GetSampler(), texture.GetImageView(), texture.GetImage().Layout())
			.UpdateDescriptors();

	}
	void ObjModel::Clean()
	{
		//m_descriptorPool->Clean();
	}
}