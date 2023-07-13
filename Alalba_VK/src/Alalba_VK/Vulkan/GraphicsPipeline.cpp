#include "pch.h"
#include "GraphicsPipeline.h"
#include "Device.h"
#include "ShaderModule.h"
#include "PipelineLayout.h"
#include "RenderPass.h"
#include "PipelineCache.h"

#include"Alalba_VK/Assets/Vertex.h"
#include "Alalba_VK/Core/ImGui/UIOverlay.h"

namespace vk
{

  GraphicsPipeline::Builder& GraphicsPipeline::Builder::AddPipelineStage(const ShaderModule& shader)
  {
    m_shaderStages.push_back(shader.GetStageCI());
    return *this;
  }

  GraphicsPipeline::Builder& GraphicsPipeline::Builder::SetVertexProcessingState(bool formodel)
  {
    m_inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    m_inputAssembly.pNext = nullptr;
    // VkPipelineInputAssemblyStateCreateFlags    flags;
    m_inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    m_inputAssembly.primitiveRestartEnable = VK_FALSE;


    m_vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    // if bufferless rendering or ImGui
    if (!formodel)
    {
      m_vertexInputState.pNext = nullptr;
      // VkPipelineVertexInputStateCreateFlags       flags;
      m_vertexInputState.vertexBindingDescriptionCount = Alalba::UIOverlay::GetBindingDescriptions().size();
      m_vertexInputState.pVertexBindingDescriptions = Alalba::UIOverlay::GetBindingDescriptions().data(); // Optional
      m_vertexInputState.vertexAttributeDescriptionCount = Alalba::UIOverlay::GetAttributeDescriptions().size();
      m_vertexInputState.pVertexAttributeDescriptions = Alalba::UIOverlay::GetAttributeDescriptions().data(); // Optional
      return *this;
    }
     
    
    m_vertexInputState.pNext = nullptr;
    // VkPipelineVertexInputStateCreateFlags       flags;
    m_vertexInputState.vertexBindingDescriptionCount = Alalba::Vertex::GetBindingDescriptions().size();
    m_vertexInputState.pVertexBindingDescriptions = Alalba::Vertex::GetBindingDescriptions().data(); 
    m_vertexInputState.vertexAttributeDescriptionCount = Alalba::Vertex::GetAttributeDescriptions().size();
    m_vertexInputState.pVertexAttributeDescriptions = Alalba::Vertex::GetAttributeDescriptions().data(); 
    
    return *this;
  }

  GraphicsPipeline::Builder& GraphicsPipeline::Builder::SetRasterizationState(VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFace, VkBool32 depthBias)
  {
    m_depthBiasOn = depthBias;

    m_rasterizerState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    m_rasterizerState.pNext = nullptr;
    //VkPipelineRasterizationStateCreateFlags    flags;
    m_rasterizerState.depthClampEnable = VK_FALSE;
    m_rasterizerState.rasterizerDiscardEnable = VK_FALSE;
    m_rasterizerState.polygonMode = polygonMode;// VK_POLYGON_MODE_LINE: VK_POLYGON_MODE_FILL;
    m_rasterizerState.cullMode = cullMode;
    // rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    m_rasterizerState.frontFace = frontFace;
    m_rasterizerState.depthBiasEnable = depthBias;
    m_rasterizerState.depthBiasConstantFactor = 0.0f; // Optional
    m_rasterizerState.depthBiasClamp = 0.0f; // Optional
    m_rasterizerState.depthBiasSlopeFactor = 0.0f; // Optional
    m_rasterizerState.lineWidth = 1.0f;
    return *this;
  }

  GraphicsPipeline::Builder& GraphicsPipeline::Builder::SetDepthState(VkBool32 depthTestEnable, VkBool32 depthWriteEnable, VkCompareOp depthCompareOp)
  {
    m_depthState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    m_depthState.pNext = nullptr;
    m_depthState.depthTestEnable = depthTestEnable;
    m_depthState.depthWriteEnable = depthWriteEnable;
    m_depthState.depthCompareOp = depthCompareOp;
    //m_depthState.back.compareOp = VK_COMPARE_OP_ALWAYS;

    m_depthState.depthBoundsTestEnable = VK_FALSE;
    m_depthState.minDepthBounds = 0.0f; // Optional
    m_depthState.maxDepthBounds = 1.0f; // Optional
    m_depthState.stencilTestEnable = VK_FALSE;
    m_depthState.front = {}; // Optional
    m_depthState.back = {}; // Optional

    return *this;
  }

  GraphicsPipeline::Builder& GraphicsPipeline::Builder::SetMultisampleState(VkSampleCountFlagBits rasterizationSamples)
  {
    m_multisamplingState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    m_multisamplingState.pNext = nullptr;
    // VkPipelineMultisampleStateCreateFlags    flags;
    m_multisamplingState.rasterizationSamples = rasterizationSamples;
    m_multisamplingState.sampleShadingEnable = VK_FALSE;
    m_multisamplingState.minSampleShading = 1.0f; // Optional
    m_multisamplingState.pSampleMask = nullptr; // Optional
    m_multisamplingState.alphaToCoverageEnable = VK_FALSE; // Optional
    m_multisamplingState.alphaToOneEnable = VK_FALSE; // Optional
    return *this;
  }

  GraphicsPipeline::Builder& GraphicsPipeline::Builder::AddColorBlendAttachmentState(VkColorComponentFlags colorWriteMask, VkBool32 blendEnable)
  {
    
    VkPipelineColorBlendAttachmentState colorAttachBlendState{};
    //colorAttachBlendState.blendEnable = blendEnable;
    //colorAttachBlendState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    //colorAttachBlendState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    //colorAttachBlendState.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    //colorAttachBlendState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    //colorAttachBlendState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    //colorAttachBlendState.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

    //TODO: following is for IMGUI, if need blending in furture, change the code below 
    colorAttachBlendState.blendEnable = blendEnable;
    colorAttachBlendState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA; 
    colorAttachBlendState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; 
    colorAttachBlendState.colorBlendOp = VK_BLEND_OP_ADD; 
    colorAttachBlendState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; 
    colorAttachBlendState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; 
    colorAttachBlendState.alphaBlendOp = VK_BLEND_OP_ADD; 

    colorAttachBlendState.colorWriteMask = colorWriteMask;

    m_colorAttachBlendStates.push_back(colorAttachBlendState);
    return *this;
  }

  GraphicsPipeline::Builder& GraphicsPipeline::Builder::SetColorBlendState(uint32_t colorAttachmentCount)
  {
    
    //ALALBA_ASSERT(colorAttachmentCount == m_colorAttachBlendStates.size(), " color attachments not match ");
    m_colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    if (colorAttachmentCount == 0)
      return *this;
     
    m_colorBlendState.pNext = nullptr;
    // VkPipelineColorBlendStateCreateFlags          flags;
    m_colorBlendState.logicOpEnable = VK_FALSE;
    m_colorBlendState.logicOp = VK_LOGIC_OP_COPY; // Optional
    m_colorBlendState.attachmentCount = m_colorAttachBlendStates.size();
    m_colorBlendState.pAttachments = m_colorAttachBlendStates.data();
    m_colorBlendState.blendConstants[0] = 0.0f; // Optional
    m_colorBlendState.blendConstants[1] = 0.0f; // Optional
    m_colorBlendState.blendConstants[2] = 0.0f; // Optional
    m_colorBlendState.blendConstants[3] = 0.0f; // Optional

    return *this;
  }

  GraphicsPipeline::Builder& GraphicsPipeline::Builder::SetViewportState(uint32_t viewportCount, uint32_t scissorCount)
  {
    m_viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    m_viewportState.viewportCount = viewportCount;
    m_viewportState.scissorCount = scissorCount;
    //m_viewportState.flags = flags;
    return *this;
  }

  GraphicsPipeline::Builder& GraphicsPipeline::Builder::SetDynamicState(VkBool32 depthBias)
  {
    ALALBA_ASSERT(depthBias == m_depthBiasOn, "depthBias should match with Rasterization state");
    m_dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    if (depthBias)
      m_dynamicStateEnables.push_back(VK_DYNAMIC_STATE_DEPTH_BIAS);

    m_dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    m_dynamicState.dynamicStateCount = static_cast<uint32_t>(m_dynamicStateEnables.size());
    m_dynamicState.pDynamicStates = m_dynamicStateEnables.data();
    m_dynamicState.flags = 0;
    m_dynamicState.pNext = nullptr;

    return *this;
  }

  GraphicsPipeline::GraphicsPipeline(const Device& device, const PipelineLayout& layout, 
    const RenderPass& renderpass, const PipelineCache& pipelineCache,
    const std::vector<VkPipelineShaderStageCreateInfo>& shaderStages,
    const VkPipelineVertexInputStateCreateInfo& vertexInputState,
    const VkPipelineInputAssemblyStateCreateInfo& inputAssembly,
    const VkPipelineRasterizationStateCreateInfo& rasterizer,
    const VkPipelineDepthStencilStateCreateInfo& depthState,
    const VkPipelineMultisampleStateCreateInfo& multisamplingState,
    const VkPipelineColorBlendStateCreateInfo& colorBlendState,
    const VkPipelineViewportStateCreateInfo& viewportState,
    const VkPipelineDynamicStateCreateInfo& dynamicState,
    const std::string& tag
  )
    :m_device(device), m_layout(layout), m_renderpass(renderpass), m_pipelineCache(pipelineCache),m_tag(tag)
  {

    ALALBA_INFO("Create Graphic Pipeline: {0}", m_tag);
    VkGraphicsPipelineCreateInfo CI{};
    CI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    CI.pNext = nullptr;
    // VkPipelineCreateFlags                            flags;
    CI.stageCount = shaderStages.size();
    CI.pStages = shaderStages.data();

    /// TODO: ***// will set after finish modeling part
    CI.pVertexInputState = &vertexInputState;
    CI.pInputAssemblyState = &inputAssembly;
    CI.pTessellationState = nullptr;
    CI.pViewportState = &viewportState;
    CI.pRasterizationState = &rasterizer;
    CI.pMultisampleState = &multisamplingState;
    CI.pDepthStencilState = &depthState;
    //CI.pDepthStencilState = nullptr;
    CI.pColorBlendState = &colorBlendState;
    CI.pDynamicState = &dynamicState;

    CI.layout = m_layout.Handle();
    CI.renderPass = m_renderpass.Handle();
    CI.subpass = 0;
    CI.basePipelineHandle = nullptr;
    CI.basePipelineIndex = -1;

    VkResult err;
    err = vkCreateGraphicsPipelines(m_device.Handle(), m_pipelineCache.Handle(), 1, &CI, nullptr, &m_pipeline);
    ALALBA_ASSERT(err == VK_SUCCESS, "Create Graphics Pipeline failed");
  }

 // GraphicsPipeline::GraphicsPipeline(const Device& device, const PipelineLayout& layout, const RenderPass& renderpass,
 //   const ShaderModule& vertex, const ShaderModule& fragment, const PipelineCache& pipelineCache,
 //   const VkPrimitiveTopology topology,
 //   const VkPolygonMode polygonMode,
 //   const bool backculling,
 //   const float viewportWidth, const float viewportHight, const VkExtent2D scirrorExtent)
	//	: m_device(device), m_layout(layout),m_renderpass(renderpass),m_vertShader(vertex),m_fragShader(fragment), m_pipelineCache(pipelineCache)
	//{
 //   ALALBA_INFO("Create Graphics Pipeline");
		/////***** shader stage
  //  std::vector<VkPipelineShaderStageCreateInfo> shaderStageCIs{};
  //  shaderStageCIs.push_back(m_vertShader.GetStageCI());
  //  shaderStageCIs.push_back(m_fragShader.GetStageCI());

    /////***** verterx input state
    //VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    //vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    //vertexInputInfo.pNext = nullptr; 
    //// VkPipelineVertexInputStateCreateFlags       flags;
    //vertexInputInfo.vertexBindingDescriptionCount = Alalba::Vertex::GetBindingDescriptions().size();
    //vertexInputInfo.pVertexBindingDescriptions = Alalba::Vertex::GetBindingDescriptions().data(); // Optional
    //vertexInputInfo.vertexAttributeDescriptionCount = Alalba::Vertex::GetAttributeDescriptions().size();
    //vertexInputInfo.pVertexAttributeDescriptions = Alalba::Vertex::GetAttributeDescriptions().data(); // Optional

    /////***** input assembly state 
    //VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    //inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    //inputAssembly.pNext = nullptr;
    //// VkPipelineInputAssemblyStateCreateFlags    flags;
    //inputAssembly.topology = topology;
    //inputAssembly.primitiveRestartEnable = VK_FALSE;

    ///*** Tessellation state
    

    /////*** Rasterization State
    //VkPipelineRasterizationStateCreateInfo rasterizer{};
    //rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    //rasterizer.pNext = nullptr;
    ////VkPipelineRasterizationStateCreateFlags    flags;
    //rasterizer.depthClampEnable = VK_FALSE;
    //rasterizer.rasterizerDiscardEnable = VK_FALSE;
    //rasterizer.polygonMode = polygonMode;// VK_POLYGON_MODE_LINE: VK_POLYGON_MODE_FILL;
    //rasterizer.cullMode = backculling ? VK_CULL_MODE_BACK_BIT : VK_CULL_MODE_NONE;
    //// rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    //rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    //rasterizer.depthBiasEnable = VK_FALSE;
    //rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    //rasterizer.depthBiasClamp = 0.0f; // Optional
    //rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
    //rasterizer.lineWidth = 1.0f;



    /////*** Depth Stencil state
    //VkPipelineDepthStencilStateCreateInfo depthStencil = {};
    //depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    //depthStencil.pNext = nullptr;
    //// VkPipelineDepthStencilStateCreateFlags    flags;
    //depthStencil.depthTestEnable = VK_TRUE; // depth
    //depthStencil.depthWriteEnable = VK_TRUE;// depth
    //depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    //depthStencil.depthBoundsTestEnable = VK_FALSE;
    //depthStencil.minDepthBounds = 0.0f; // Optional
    //depthStencil.maxDepthBounds = 1.0f; // Optional
    //depthStencil.stencilTestEnable = VK_FALSE;
    //depthStencil.front = {}; // Optional
    //depthStencil.back = {}; // Optional


    /////*** Multisample State
    //VkPipelineMultisampleStateCreateInfo multisampling{};
    //multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    //multisampling.pNext = nullptr;
    //// VkPipelineMultisampleStateCreateFlags    flags;
    //multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    //multisampling.sampleShadingEnable = VK_FALSE;
    //multisampling.minSampleShading = 1.0f; // Optional
    //multisampling.pSampleMask = nullptr; // Optional
    //multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    //multisampling.alphaToOneEnable = VK_FALSE; // Optional


     ///*** Color Blend
    ///// TODO: for now there's no blend between each attachment
    //VkPipelineColorBlendAttachmentState colorBlendATCHState = {};
    //colorBlendATCHState.blendEnable = VK_FALSE;
    //colorBlendATCHState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    //colorBlendATCHState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    //colorBlendATCHState.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    //colorBlendATCHState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    //colorBlendATCHState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    //colorBlendATCHState.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
    //colorBlendATCHState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
    //  | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;


    //// each color attachment in renderpass should has one ColorBlendAttachmentState

    //std::vector<VkPipelineColorBlendAttachmentState>colorAttachBlends(m_renderpass.ColorAttachmentCount());
    //for (auto& blend : colorAttachBlends)
    //{
    //  blend = colorBlendATCHState;
    //}

    //VkPipelineColorBlendStateCreateInfo colorBlendingState = {};
    //colorBlendingState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    //colorBlendingState.pNext = nullptr;
    //// VkPipelineColorBlendStateCreateFlags          flags;
    //colorBlendingState.logicOpEnable = VK_FALSE;
    //colorBlendingState.logicOp = VK_LOGIC_OP_COPY; // Optional
    //colorBlendingState.attachmentCount = colorAttachBlends.size();
    //colorBlendingState.pAttachments = colorAttachBlends.data();
    //colorBlendingState.blendConstants[0] = 0.0f; // Optional
    //colorBlendingState.blendConstants[1] = 0.0f; // Optional
    //colorBlendingState.blendConstants[2] = 0.0f; // Optional
    //colorBlendingState.blendConstants[3] = 0.0f; // Optional

    /////*** View port state
    //VkViewport viewport = {};
    //viewport.x = 0.0f;
    //viewport.y = 0.0f;
    //viewport.width = viewportWidth;
    //viewport.height = viewportHight;
    //viewport.minDepth = 0.0f;
    //viewport.maxDepth = 1.0f;

    //VkRect2D scissor = {};
    //scissor.offset = { 0, 0 };
    //scissor.extent = scirrorExtent;

    //VkPipelineViewportStateCreateInfo viewportState {};
    //viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    //viewportState.pNext = nullptr;
    //// VkPipelineViewportStateCreateFlags    flags;
    //viewportState.viewportCount = 1;
    //viewportState.pViewports = &viewport;
    //viewportState.scissorCount = 1;
    //viewportState.pScissors = &scissor;


    ///// ******* Dynamic State
    //std::vector<VkDynamicState> dynamicStates = {
    //  VK_DYNAMIC_STATE_VIEWPORT,
    //  VK_DYNAMIC_STATE_SCISSOR
    //};
    //VkPipelineDynamicStateCreateInfo dynamicState{};
    //dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    //dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    //dynamicState.pDynamicStates = dynamicStates.data();
    //dynamicState.flags = 0;
    //dynamicState.pNext = nullptr;

	//	VkGraphicsPipelineCreateInfo CI{};
 //   CI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
 //   CI.pNext = nullptr;
 //   // VkPipelineCreateFlags                            flags;
 //   CI.stageCount = shaderStageCIs.size();
 //   CI.pStages= shaderStageCIs.data();
 //   
 //   /// TODO: ***// will set after finish modeling part
 //   CI.pVertexInputState = &vertexInputInfo;
 //   CI.pInputAssemblyState = &inputAssembly;
 //   CI.pTessellationState = nullptr;
 //   CI.pViewportState = &viewportState;
 //   CI.pRasterizationState = &rasterizer;
 //   CI.pMultisampleState = &multisampling;
 //   CI.pDepthStencilState = &depthStencil;
 //   //CI.pDepthStencilState = nullptr;
 //   CI.pColorBlendState = &colorBlendingState;
 //   CI.pDynamicState = &dynamicState;
 //   
 //   CI.layout = m_layout.Handle();
 //   CI.renderPass = m_renderpass.Handle();
 //   CI.subpass = 0;
 //   CI.basePipelineHandle = nullptr;
 //   CI.basePipelineIndex = -1;

	//	VkResult err;
 //   err = vkCreateGraphicsPipelines(m_device.Handle(), m_pipelineCache.Handle(), 1, &CI, nullptr, &m_pipeline);
 //   ALALBA_ASSERT(err == VK_SUCCESS, "Create Graphics Pipeline failed");
	//}

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