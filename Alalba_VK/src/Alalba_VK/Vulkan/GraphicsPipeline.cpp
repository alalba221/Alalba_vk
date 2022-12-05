#include "pch.h"
#include "GraphicsPipeline.h"
#include "Device.h"
#include "ShaderModule.h"
#include "PipelineLayout.h"
#include "RenderPass.h"
#include"Alalba_VK/Assets/Vertex.h"
namespace vk
{
	GraphicsPipeline::GraphicsPipeline(const Device& device, const PipelineLayout& layout, const RenderPass& renderpass,
    const ShaderModule& vertex, const ShaderModule& fragment,
    const VkPrimitiveTopology topology,
    const VkPolygonMode polygonMode,
    const bool backculling,
    const float viewportWidth, const float viewportHight, const VkExtent2D scirrorExtent)
		: m_device(device), m_layout(layout),m_renderpass(renderpass),m_vertShader(vertex),m_fragShader(fragment)
	{
    ALALBA_INFO("Create Graphics Pipeline");
		///***** shader stage
    std::vector<VkPipelineShaderStageCreateInfo> shaderStageCIs{};
    shaderStageCIs.push_back(m_vertShader.GetStageCI());
    shaderStageCIs.push_back(m_fragShader.GetStageCI());

    ///***** verterx input state
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.pNext = nullptr; 
    // VkPipelineVertexInputStateCreateFlags       flags;
    vertexInputInfo.vertexBindingDescriptionCount = Alalba::MeshVertex::GetBindingDescriptions().size();
    vertexInputInfo.pVertexBindingDescriptions = Alalba::MeshVertex::GetBindingDescriptions().data(); // Optional
    vertexInputInfo.vertexAttributeDescriptionCount = Alalba::MeshVertex::GetAttributeDescriptions().size();
    vertexInputInfo.pVertexAttributeDescriptions = Alalba::MeshVertex::GetAttributeDescriptions().data(); // Optional

    ///***** input assembly state 
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.pNext = nullptr;
    // VkPipelineInputAssemblyStateCreateFlags    flags;
    inputAssembly.topology = topology;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    ///*** Tessellation state
    
    ///*** View port state
    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = viewportWidth;
    viewport.height = viewportHight;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent = scirrorExtent;

    VkPipelineViewportStateCreateInfo viewportState {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.pNext = nullptr;
    // VkPipelineViewportStateCreateFlags    flags;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    ///*** Rasterization State
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.pNext = nullptr;
    //VkPipelineRasterizationStateCreateFlags    flags;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = polygonMode;// VK_POLYGON_MODE_LINE: VK_POLYGON_MODE_FILL;
    rasterizer.cullMode = backculling?VK_CULL_MODE_BACK_BIT: VK_CULL_MODE_NONE;
    // rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f; // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
    rasterizer.lineWidth = 1.0f;

    ///*** Multisample State
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.pNext = nullptr;
    // VkPipelineMultisampleStateCreateFlags    flags;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = nullptr; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional

    ///*** Depth Stencil state
    VkPipelineDepthStencilStateCreateInfo depthStencil = {};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.pNext = nullptr;
    // VkPipelineDepthStencilStateCreateFlags    flags;
    depthStencil.depthTestEnable = VK_TRUE; // depth
    depthStencil.depthWriteEnable = VK_TRUE;// depth
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f; // Optional
    depthStencil.maxDepthBounds = 1.0f; // Optional
    depthStencil.stencilTestEnable = VK_FALSE;
    depthStencil.front = {}; // Optional
    depthStencil.back = {}; // Optional

    ///*** Color Blend
    VkPipelineColorBlendAttachmentState colorBlendATCHState = {};
    colorBlendATCHState.blendEnable = VK_FALSE;
    colorBlendATCHState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendATCHState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendATCHState.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    colorBlendATCHState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendATCHState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendATCHState.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
    colorBlendATCHState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
      | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendStateCreateInfo colorBlendingState = {};
    colorBlendingState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendingState.pNext = nullptr;
    // VkPipelineColorBlendStateCreateFlags          flags;
    colorBlendingState.logicOpEnable = VK_FALSE;
    colorBlendingState.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlendingState.attachmentCount = 1;
    colorBlendingState.pAttachments = &colorBlendATCHState;
    colorBlendingState.blendConstants[0] = 0.0f; // Optional
    colorBlendingState.blendConstants[1] = 0.0f; // Optional
    colorBlendingState.blendConstants[2] = 0.0f; // Optional
    colorBlendingState.blendConstants[3] = 0.0f; // Optional


    /// ******* Dynamic State
    std::vector<VkDynamicState> dynamicStates = {
      VK_DYNAMIC_STATE_VIEWPORT,
      VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();
    dynamicState.flags = 0;
    dynamicState.pNext = nullptr;

		VkGraphicsPipelineCreateInfo CI{};
    CI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    CI.pNext = nullptr;
    // VkPipelineCreateFlags                            flags;
    CI.stageCount = shaderStageCIs.size();
    CI.pStages= shaderStageCIs.data();
    
    /// TODO: ***// will set after finish modeling part
    CI.pVertexInputState = &vertexInputInfo;
    CI.pInputAssemblyState = &inputAssembly;
    CI.pTessellationState = nullptr;
    CI.pViewportState = &viewportState;
    CI.pRasterizationState = &rasterizer;
    CI.pMultisampleState = &multisampling;
    CI.pDepthStencilState = &depthStencil;
    //CI.pDepthStencilState = nullptr;
    CI.pColorBlendState = &colorBlendingState;
    CI.pDynamicState = &dynamicState;
    
    CI.layout = m_layout.Handle();
    CI.renderPass = m_renderpass.Handle();
    CI.subpass = 0;
    CI.basePipelineHandle = nullptr;
    CI.basePipelineIndex = -1;

		VkResult err;
    err = vkCreateGraphicsPipelines(m_device.Handle(), nullptr, 1, &CI, nullptr, &m_pipeline);
    ALALBA_ASSERT(err == VK_SUCCESS, "Create Graphics Pipeline failed");
	}

  void GraphicsPipeline::Clean()
  {
    if (m_pipeline != VK_NULL_HANDLE)
    {
      ALALBA_WARN("Clean Graphics Pipeline {0}", m_tag);
      vkDestroyPipeline(m_device.Handle(), m_pipeline, nullptr);
      m_pipeline = VK_NULL_HANDLE;
    }
  }

}