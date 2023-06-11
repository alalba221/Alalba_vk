#include "pch.h"
#include "glTFRenderSys.h"
#include "Alalba_VK/Vulkan/DescriptorSetLayout.h"
#include "Alalba_VK/Vulkan/PipelineCache.h"
#include "Alalba_VK/Core/Application.h"
#include "Alalba_VK/Vulkan/CommandBuffers.h"
#include "Alalba_VK/Core/Scene/Entity.h"

namespace Alalba
{
	glTFRenderSys::glTFRenderSys(Scene& scene,
		const vk::RenderPass& renderpass, const std::vector<const vk::DescriptorSetLayout*>& descriptorSetLayouts, const vk::PipelineCache& pipelineCache)
		:m_scene(scene)
	{
		CreateShaders("shaders/gltf.vert.spv", "shaders/gltf.frag.spv");
		CreatePipelineLayout(descriptorSetLayouts);
		CreatePipeline(renderpass, pipelineCache);

	}

	void glTFRenderSys::Render(vk::CommandBuffers& cmdBuffers,
		const vk::DescriptorSet& globalDescSet, const int currentCmdBuffer)
	{
		// bind pipeline
		vkCmdBindPipeline(cmdBuffers[currentCmdBuffer], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline->Handle());
		// bind global descriptor set
		VkDescriptorSet gbset = globalDescSet.Handle();
		vkCmdBindDescriptorSets(cmdBuffers[currentCmdBuffer], VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_pipelineLayout->Handle(), 0, 1, &gbset, 0, nullptr);

		auto view = m_scene.GetAllEntitiesWith<GLTFComponent, TransformComponent>();

		/// for each entity
		for (auto entity : view)
		{
			auto model = view.get<GLTFComponent>(entity).Model;
			auto basetransform = view.get<TransformComponent>(entity).Transform;

			DrawModel(*model, basetransform, cmdBuffers, currentCmdBuffer);

		}
	}

	// private
	void glTFRenderSys::CreateShaders(const std::string& vertex, const std::string& frag)
	{
		const vk::Device& device = Application::Get().GetDevice();

		// create shader
		m_vertexShader = vk::ShaderModule::Builder(device)
			// path relative to Sandbox
			.SetTag("glTF vertex Shader")
			.SelectSpvFile(vertex)
			.SetShaderStageBits(VK_SHADER_STAGE_VERTEX_BIT)
			.Build();

		m_fragShader = vk::ShaderModule::Builder(device)
			// path relative to Sandbox
			.SetTag("glTF fragShader")
			.SelectSpvFile(frag)
			.SetShaderStageBits(VK_SHADER_STAGE_FRAGMENT_BIT)
			.Build();
	}

	void glTFRenderSys::CreatePipelineLayout(const std::vector<const vk::DescriptorSetLayout*>& descriptorSetLayouts)
	{
		const vk::Device& device = Application::Get().GetDevice();

		m_pipelineLayout = vk::PipelineLayout::Builder(device)
			.SetTag("gltf pipeline layout")
			.AddPushConstant(sizeof(glm::mat4))
			.BindDescriptorSetLayout(*descriptorSetLayouts[0])
			.BindDescriptorSetLayout(*descriptorSetLayouts[1])
			.Build()
			;

	}

	void glTFRenderSys::CreatePipeline(const vk::RenderPass& renderpass, const vk::PipelineCache& pipelineCache)
	{
		const vk::Device& device = Application::Get().GetDevice();

		m_graphicsPipeline = vk::GraphicsPipeline::Builder(device, *m_pipelineLayout, renderpass,
			*m_vertexShader, *m_fragShader, pipelineCache)
			.SetAssemblyTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
			.SetPolygonMode(VK_POLYGON_MODE_FILL)
			.SetBackCulling(false)
			.Build();

	}

	void glTFRenderSys::DrawModel(const GLTFModel& model, const glm::mat4& basetransform, vk::CommandBuffers& cmdBuffers, const int currentCmdBuffer)
	{
		// 1. Get vertex and index buffer
		VkBuffer vertexBuffers[] = { model.GetVertexBuffer().Handle() };
		VkBuffer IndexBuffers = model.GetIndexBuffer().Handle();
		
		// 2. Bind Vertex and index buffer
		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(cmdBuffers[currentCmdBuffer], 0, 1, vertexBuffers, &offset);
		vkCmdBindIndexBuffer(cmdBuffers[currentCmdBuffer], IndexBuffers, offset, VK_INDEX_TYPE_UINT32);

		std::vector<Node*> nodes = model.GetNodes();
		for (auto node : nodes)
		{
			DrawNode(model, basetransform, node, cmdBuffers, currentCmdBuffer);
		}
	}

	void glTFRenderSys::DrawNode(const GLTFModel& model, const glm::mat4& basetransform, const Node* node, vk::CommandBuffers& cmdBuffers, const int currentCmdBuffer)
	{
		if (node->mesh) 
		{
			glm::mat4 nodeMatrix = node->matrix;
			Node* currentParent = node->parent;
			while (currentParent) 
			{
				nodeMatrix = currentParent->matrix * nodeMatrix;
				currentParent = currentParent->parent;
			}

			// Pass the final matrix to the vertex shader using push constants
			nodeMatrix = basetransform * nodeMatrix;
			vkCmdPushConstants(cmdBuffers[currentCmdBuffer], m_pipelineLayout->Handle(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &nodeMatrix);
			for (Primitive* primitive : node->mesh->primitives) {
				// bool skip = false;
				uint32_t materialIndex = primitive->materialIndex;
				//if (renderFlags & RenderFlags::RenderOpaqueNodes) {
				//	skip = (material.alphaMode != Material::ALPHAMODE_OPAQUE);
				//}
				//if (renderFlags & RenderFlags::RenderAlphaMaskedNodes) {
				//	skip = (material.alphaMode != Material::ALPHAMODE_MASK);
				//}
				//if (renderFlags & RenderFlags::RenderAlphaBlendedNodes) {
				//	skip = (material.alphaMode != Material::ALPHAMODE_BLEND);
				//}
				//if (!skip) {
				//	if (renderFlags & RenderFlags::BindImages) {
				//		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, bindImageSet, 1, &material.descriptorSet, 0, nullptr);
				//	}
				//	vkCmdDrawIndexed(commandBuffer, primitive->indexCount, 1, primitive->firstIndex, 0, 0);
				//}

				//
				uint32_t materialID = primitive->materialIndex;
				Material* material = model.GetMatirials().at(materialID);
				VkDescriptorSet materialSet = material->descSet->Handle();
				vkCmdBindDescriptorSets(cmdBuffers[currentCmdBuffer], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout->Handle(), 1, 1, &materialSet, 0, nullptr);
				vkCmdDrawIndexed(cmdBuffers[currentCmdBuffer], primitive->indexCount, 1, primitive->firstIndex, 0, 0);
			}
		}
		for (auto& child : node->children) {
			DrawNode(model, basetransform, child, cmdBuffers, currentCmdBuffer);
		}
	}

	void glTFRenderSys::Update()
	{


	}
	void glTFRenderSys::ShutDown()
	{
		ALALBA_ERROR("shut downgltf system");
		m_vertexShader->Clean();
		m_fragShader->Clean();
		m_pipelineLayout->Clean();
		m_graphicsPipeline->Clean();
	}

}
