#include "pch.h"
#include "DiffractionSys.h"
#include "Alalba_VK/Vulkan/DescriptorSetLayout.h"
#include "Alalba_VK/Vulkan/PipelineCache.h"
#include "Alalba_VK/Core/Application.h"
#include "Alalba_VK/Vulkan/CommandBuffers.h"
#include "Alalba_VK/Core/Scene/Entity.h"
namespace Alalba
{
	DiffractionSys::DiffractionSys(Scene& scene,
		const vk::RenderPass& renderpass, const std::vector<const vk::DescriptorSetLayout*>& descriptorSetLayouts, const vk::PipelineCache& pipelineCache)
	{
		CreateShaders("shaders/diffraction.vert.spv", "shaders/diffraction.frag.spv");
		CreatePipelineLayout(descriptorSetLayouts);
		CreatePipeline(renderpass, pipelineCache);
		PrepareDescriptorSets(scene, descriptorSetLayouts);
	}

	void DiffractionSys::Render(Scene& scene, vk::CommandBuffers& cmdBuffers,
		const vk::DescriptorSet& globalDescSet, const int currentCmdBuffer)
	{
		// bind pipeline
		vkCmdBindPipeline(cmdBuffers[currentCmdBuffer], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline->Handle());
		// bind global descriptor set
		VkDescriptorSet gbset = globalDescSet.Handle();
		vkCmdBindDescriptorSets(cmdBuffers[currentCmdBuffer], VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_pipelineLayout->Handle(), 0, 1, &gbset, 0, nullptr);

		auto view = scene.GetAllEntitiesWith<MeshComponent>();

		/// for each entity
		for (auto entity : view)
		{
			auto mesh = view.get<MeshComponent>(entity);
			std::shared_ptr<Mesh> pmesh = mesh.m_Mesh;
			// 1. Get vertex and index buffer
			VkBuffer vertexBuffers[] = { pmesh->GetVertexbuffer().Handle() };
			VkBuffer IndexBuffers = pmesh->GetIndexbuffer().Handle();

			/// for currend command buffer, record command 
			{
				// TODO: 2. PUSHCONSTANT for model matrix
				// 3. bind vectex and index buffer to pipeline
				VkDeviceSize offset = 0;
				vkCmdBindVertexBuffers(cmdBuffers[currentCmdBuffer], 0, 1, vertexBuffers, &offset);
				vkCmdBindIndexBuffer(cmdBuffers[currentCmdBuffer], IndexBuffers, offset, VK_INDEX_TYPE_UINT32);

				// 4. bind local descriptors to pipeline 
				// start from index 1
				//VkDescriptorSet modset = descriptorset.DescriptorSet->Handle();
				//vkCmdBindDescriptorSets(cmdBuffers[currentCmdBuffer], VK_PIPELINE_BIND_POINT_GRAPHICS,
				//	m_pipelineLayout->Handle(), 1, 1, &modset, 0, nullptr);

				// 5. draw
				vkCmdDrawIndexed(cmdBuffers[currentCmdBuffer], pmesh->GetIndexCount(), pmesh->GetInstanceCount(), 0, 0, 0);
			}
		}
	}

	// private
	void DiffractionSys::CreateShaders(const std::string& vertex, const std::string& frag)
	{
		const vk::Device& device = Application::Get().GetDevice();

		// create shader
		m_vertexShader = vk::ShaderModule::Builder(device)
			// path relative to Sandbox
			.SetTag("basic vertex Shader")
			.SelectSpvFile(vertex)
			.SetShaderStageBits(VK_SHADER_STAGE_VERTEX_BIT)
			.Build();

		m_fragShader = vk::ShaderModule::Builder(device)
			// path relative to Sandbox
			.SetTag("basic fragShader")
			.SelectSpvFile(frag)
			.SetShaderStageBits(VK_SHADER_STAGE_FRAGMENT_BIT)
			.Build();
	}

	void DiffractionSys::CreatePipelineLayout(const std::vector<const vk::DescriptorSetLayout*>& descriptorSetLayouts)
	{
		const vk::Device& device = Application::Get().GetDevice();
		m_pipelineLayout = std::make_unique<vk::PipelineLayout>(device, descriptorSetLayouts, "basic pipeline layout");
	}

	void DiffractionSys::CreatePipeline(const vk::RenderPass& renderpass, const vk::PipelineCache& pipelineCache)
	{
		const vk::Device& device = Application::Get().GetDevice();

		m_graphicsPipeline = vk::GraphicsPipeline::Builder(device, *m_pipelineLayout, renderpass,
			*m_vertexShader, *m_fragShader, pipelineCache)
			.SetAssemblyTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
			.SetPolygonMode(VK_POLYGON_MODE_FILL)
			.SetBackCulling(false)
			/*.SetScirrorExtent(m_swapChain->GetExtent())
			.SetViewPortWidth(static_cast<float>(m_SwapChain->GetExtent().width))
			.SetViewPortHeight(static_cast<float>(m_SwapChain->GetExtent().height))*/
			.Build();
	}
	void DiffractionSys::PrepareDescriptorSets(Scene& scene, const std::vector<const vk::DescriptorSetLayout*>& descriptorSetLayouts)
	{
		const vk::Device& device = Application::Get().GetDevice();

		m_descPool = vk::DescriptorPool::Builder(device)
			.SetTag("Bsic Descriptor Pool")
			.SetMaxSets(1000)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000)
			.Build();


		auto view = scene.GetAllEntitiesWith<TextureComponent>();
		
		// Add descriptor set component to each entity
		//for (auto e : view)
		//{
		//	Entity entity{ e,&scene };
		//	auto& entity_tag = entity.GetComponent<TagComponent>();
		//	std::string tag = entity_tag.Tag + " descriptor set";

		//	// use the second descriptor set layout : descriptorSetLayouts[1]
		//	m_textureDescSets.push_back(
		//		std::make_shared<vk::DescriptorSet>(device, *m_descPool, descriptorSetLayouts[1], tag)
		//	);

		//	std::shared_ptr<Texture> ptexture = entity.GetComponent<TextureComponent>().m_Texture;
		//	m_textureDescSets.back()->BindDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0,
		//		ptexture->GetSampler(), ptexture->GetImageView(), ptexture->GetImage().Layout());
		//	m_textureDescSets.back()->UpdateDescriptors();

		//	entity.AddComponent<DescrpotrSetCompont>(m_textureDescSets.back());
		//}
	}
	void DiffractionSys::ShutDown()
	{
		ALALBA_ERROR("shut down diffraction system");
		m_vertexShader->Clean();
		m_fragShader->Clean();
		m_pipelineLayout->Clean();
		m_graphicsPipeline->Clean();
		m_descPool->Clean();
	}
}
