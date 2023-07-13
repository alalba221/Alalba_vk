#include "pch.h"
#include "UIOverlay.h"
#include "imgui.h"
#include "Alalba_VK/Vulkan/RenderPass.h"
#include "Alalba_VK/Core/Application.h"
#include "Alalba_VK/Vulkan/CommandBuffers.h"
namespace Alalba
{
	std::array<VkVertexInputBindingDescription, 1> UIOverlay::binding_descriptions =
	{
		VkVertexInputBindingDescription{}
	};

	std::array<VkVertexInputAttributeDescription, 3> UIOverlay::attribute_descriptions =
	{
		VkVertexInputAttributeDescription{}, VkVertexInputAttributeDescription{},
		VkVertexInputAttributeDescription{}
	};


	UIOverlay::UIOverlay(const vk::RenderPass& renderPass)
		:m_renderPass(renderPass)
	{
		Application& app = Application::Get();
		const vk::Device& device = Application::Get().GetDevice();
		

		// Init ImGui
		ImGui::CreateContext();

		// Color scheme


		// Dimensions
		ImGuiIO& io = ImGui::GetIO();
#if defined(_WIN32)
		// If we directly work with os specific key codes, we need to map special key types like tab
		io.KeyMap[ImGuiKey_Tab] = VK_TAB;
		io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
		io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
		io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
		io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
		io.KeyMap[ImGuiKey_Space] = VK_SPACE;
		io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
#endif
		io.DisplaySize = ImVec2((float)1280, (float)720);
		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

		// Color scheme
		ImGuiStyle& style = ImGui::GetStyle();
		style.Colors[ImGuiCol_TitleBg] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.0f, 0.0f, 0.0f, 0.1f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.8f, 0.0f, 0.0f, 0.4f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.8f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(1.0f, 0.0f, 0.0f, 0.8f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.0f, 0.0f, 0.0f, 0.8f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(1.0f, 1.0f, 1.0f, 0.1f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(1.0f, 1.0f, 1.0f, 0.2f);
		style.Colors[ImGuiCol_Button] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(1.0f, 0.0f, 0.0f, 0.6f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(1.0f, 0.0f, 0.0f, 0.8f);


		// create shader
		m_vertexShader = vk::ShaderModule::Builder(device)
			// path relative to Sandbox
			.SetTag("UI vertex Shader")
			.SelectSpvFile("shaders/ui.vert.spv")
			.SetShaderStageBits(VK_SHADER_STAGE_VERTEX_BIT)
			.Build();

		m_fragShader = vk::ShaderModule::Builder(device)
			// path relative to Sandbox
			.SetTag("UI fragShader")
			.SelectSpvFile("shaders/ui.frag.spv")
			.SetShaderStageBits(VK_SHADER_STAGE_FRAGMENT_BIT)
			.Build();

		PrepareResources();
		m_pipelineCache = vk::PipelineCache::Builder(device)
			.SetTag("UI PipelineCache")
			.Build();
		PreparePipeline();
	}
	void UIOverlay::NewFrame()
	{
		

	/*	io.MousePos = ImVec2(mousePos.x, mousePos.y);
		io.MouseDown[0] = mouseButtons.left && UIOverlay.visible;
		io.MouseDown[1] = mouseButtons.right && UIOverlay.visible;
		io.MouseDown[2] = mouseButtons.middle && UIOverlay.visible;*/
//		if (!visible)
//			return;
//
//		ImGuiIO& io = ImGui::GetIO();
//
//		//io.DisplaySize = ImVec2((float)width, (float)height);
//		//io.DeltaTime = frameTimer;
//
//		//io.MousePos = ImVec2(mousePos.x, mousePos.y);
//		//io.MouseDown[0] = mouseButtons.left && UIOverlay.visible;
//		//io.MouseDown[1] = mouseButtons.right && UIOverlay.visible;
//		//io.MouseDown[2] = mouseButtons.middle && UIOverlay.visible;
//
//		ImGui::NewFrame();
//
//		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
//		ImGui::SetNextWindowPos(ImVec2(10 * UIOverlay.scale, 10 * UIOverlay.scale));
//		ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiSetCond_FirstUseEver);
//		ImGui::Begin("Vulkan Example", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
//		ImGui::TextUnformatted(title.c_str());
//		ImGui::TextUnformatted(deviceProperties.deviceName);
//		ImGui::Text("%.2f ms/frame (%.1d fps)", (1000.0f / lastFPS), lastFPS);
//
//#if defined(VK_USE_PLATFORM_ANDROID_KHR)
//		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 5.0f * UIOverlay.scale));
//#endif
//		ImGui::PushItemWidth(110.0f * UIOverlay.scale);
//		OnUpdateUIOverlay(&UIOverlay);
//		ImGui::PopItemWidth();
//#if defined(VK_USE_PLATFORM_ANDROID_KHR)
//		ImGui::PopStyleVar();
//#endif
//
//		ImGui::End();
//		ImGui::PopStyleVar();
//		ImGui::Render();
//
//		if (UIOverlay.update() || UIOverlay.updated) {
//			buildCommandBuffers();
//			UIOverlay.updated = false;
//		}

		ImGui::NewFrame();

		// Init imGui windows and elements

		// Debug window
		ImGui::SetWindowPos(ImVec2(20 * scale, 20 * scale), ImGuiCond_FirstUseEver);
		ImGui::SetWindowSize(ImVec2(300 * scale, 300 * scale), ImGuiCond_Always);
		ImGui::TextUnformatted("Vulkan Example");
		ImGui::TextUnformatted("Vulkan dEVICE NAME");

		////SRS - Display Vulkan API version and device driver information if available (otherwise blank)
		//ImGui::Text("Vulkan API %i.%i.%i", 1,1,1);
		//ImGui::Text("%s %s", "driver name", "driver info");

		//// Update frame time display
		///*if (updateFrameGraph) {
		//	std::rotate(uiSettings.frameTimes.begin(), uiSettings.frameTimes.begin() + 1, uiSettings.frameTimes.end());
		//	float frameTime = 1000.0f / (example->frameTimer * 1000.0f);
		//	uiSettings.frameTimes.back() = frameTime;
		//	if (frameTime < uiSettings.frameTimeMin) {
		//		uiSettings.frameTimeMin = frameTime;
		//	}
		//	if (frameTime > uiSettings.frameTimeMax) {
		//		uiSettings.frameTimeMax = frameTime;
		//	}
		//}*/

		////ImGui::PlotLines("Frame Times", &uiSettings.frameTimes[0], 50, 0, "", uiSettings.frameTimeMin, uiSettings.frameTimeMax, ImVec2(0, 80));

		//ImGui::Text("Camera");
		////ImGui::InputFloat3("position", &example->camera.position.x, 2);
		////ImGui::InputFloat3("rotation", &example->camera.rotation.x, 2);

		//// Example settings window
		//ImGui::SetNextWindowPos(ImVec2(20 * scale, 360 * scale), ImGuiCond_FirstUseEver);
		//ImGui::SetNextWindowSize(ImVec2(300 * scale, 200 * scale), ImGuiCond_FirstUseEver);
		//ImGui::Begin("Example settings");
		/*ImGui::Checkbox("Render models", nullptr);
		ImGui::Checkbox("Display logos", nullptr);
		ImGui::Checkbox("Display background", nullptr);
		ImGui::Checkbox("Animate light", nullptr);
		ImGui::SliderFloat("Light speed", nullptr, 0.1f, 1.0f);*/
		//ImGui::ShowStyleSelector("UI style");

		//if (ImGui::Combo("UI style", nullptr, "Vulkan\0Classic\0Dark\0Light\0")) {
		//	//setStyle(selectedStyle);
		//}

		//ImGui::End();

		//SRS - ShowDemoWindow() sets its own initial position and size, cannot override here
		ImGui::ShowDemoWindow();

		// Render to generate draw buffers
		ImGui::Render();
	}
	bool UIOverlay::UpdateBuffers()
	{
		const vk::Device& device = Application::Get().GetDevice();

		ImDrawData* imDrawData = ImGui::GetDrawData();
		bool updateCmdBuffers = false;
		if (!imDrawData) { return false; };


		// Note: Alignment is done inside buffer creation
		VkDeviceSize vertexBufferSize = imDrawData->TotalVtxCount * sizeof(ImDrawVert);
		VkDeviceSize indexBufferSize = imDrawData->TotalIdxCount * sizeof(ImDrawIdx);

		if ((vertexBufferSize == 0) || (indexBufferSize == 0)) {
			return false;
		}

		// Update buffers only if vertex or index count has been changed compared to current buffer size

		// Vertex buffer
		if ((!m_vertexBuffer) || (vertexCount != imDrawData->TotalVtxCount)) {
			/*vertexBuffer.unmap();
			vertexBuffer.destroy();*/
			m_vertexBuffer.reset(new vk::Buffer(device, *m_allocator, vertexBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, "UI Vertex Buffer"));
			vertexCount = imDrawData->TotalVtxCount;
			m_vertexBuffer->MapMemory();
			updateCmdBuffers = true;
		}

		// Index buffer
		if ((!m_indexBuffer) || (indexCount < imDrawData->TotalIdxCount)) {
		/*	indexBuffer.unmap();
			indexBuffer.destroy();*/
			m_indexBuffer.reset(new vk::Buffer(device, *m_allocator, indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, "UI Index Buffer"));
			indexCount = imDrawData->TotalIdxCount;
			m_indexBuffer->MapMemory();
			updateCmdBuffers = true;
		}

		// Upload data
		ImDrawVert* vtxDst = (ImDrawVert*)m_vertexBuffer->Mapped();
		ImDrawIdx* idxDst = (ImDrawIdx*)m_indexBuffer->Mapped();

		for (int n = 0; n < imDrawData->CmdListsCount; n++) 
		{
			const ImDrawList* cmd_list = imDrawData->CmdLists[n];
			memcpy(vtxDst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
			memcpy(idxDst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
			vtxDst += cmd_list->VtxBuffer.Size;
			idxDst += cmd_list->IdxBuffer.Size;
		}

		// Flush to make writes visible to GPU
		m_vertexBuffer->Flush();
		m_indexBuffer->Flush();

		return updateCmdBuffers;
	}
	void UIOverlay::Draw(const vk::CommandBuffers& cmdbuffers, const uint32_t currentCmdBuffer)
	{
		ImDrawData* imDrawData = ImGui::GetDrawData();
		int32_t vertexOffset = 0;
		int32_t indexOffset = 0;

		if ((!imDrawData) || (imDrawData->CmdListsCount == 0)) {
			return;
		}

		ImGuiIO& io = ImGui::GetIO();
		VkDescriptorSet descset = m_descriptorSet->Handle();
		vkCmdBindDescriptorSets(cmdbuffers[currentCmdBuffer], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout->Handle(), 0, 1, &descset, 0, nullptr);
		vkCmdBindPipeline(cmdbuffers[currentCmdBuffer], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->Handle());

	
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = ImGui::GetIO().DisplaySize.x;
		viewport.height = ImGui::GetIO().DisplaySize.y;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(cmdbuffers[currentCmdBuffer], 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent.width = ImGui::GetIO().DisplaySize.x;
		scissor.extent.height = ImGui::GetIO().DisplaySize.y;
		vkCmdSetScissor(cmdbuffers[currentCmdBuffer], 0, 1, &scissor);

		// UI scale and translate via push constants
		pushConstBlock.scale = glm::vec2(2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y);
		pushConstBlock.translate = glm::vec2(-1.0f);
		vkCmdPushConstants(cmdbuffers[currentCmdBuffer], m_pipelineLayout->Handle(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstBlock), &pushConstBlock);

		// Render commands

		if (imDrawData->CmdListsCount > 0) {
			VkBuffer vertexBuffers[] = { m_vertexBuffer->Handle() };
			VkBuffer IndexBuffers = m_indexBuffer->Handle();
			VkDeviceSize offsets[1] = { 0 };

			vkCmdBindVertexBuffers(cmdbuffers[currentCmdBuffer], 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(cmdbuffers[currentCmdBuffer], IndexBuffers, 0, VK_INDEX_TYPE_UINT16);

			for (int32_t i = 0; i < imDrawData->CmdListsCount; i++)
			{
				const ImDrawList* cmd_list = imDrawData->CmdLists[i];
				for (int32_t j = 0; j < cmd_list->CmdBuffer.Size; j++)
				{
					const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[j];
					VkRect2D scissorRect;
					scissorRect.offset.x = std::max((int32_t)(pcmd->ClipRect.x), 0);
					scissorRect.offset.y = std::max((int32_t)(pcmd->ClipRect.y), 0);
					scissorRect.extent.width = (uint32_t)(pcmd->ClipRect.z - pcmd->ClipRect.x);
					scissorRect.extent.height = (uint32_t)(pcmd->ClipRect.w - pcmd->ClipRect.y);
					vkCmdSetScissor(cmdbuffers[currentCmdBuffer], 0, 1, &scissorRect);
					vkCmdDrawIndexed(cmdbuffers[currentCmdBuffer], pcmd->ElemCount, 1, indexOffset, vertexOffset, 0);
					indexOffset += pcmd->ElemCount;
				}
				vertexOffset += cmd_list->VtxBuffer.Size;
			}
		}
	}
	
	/*void UIOverlay::Draw(const vk::CommandBuffers& cmdbuffers, const int currentCmdBuffer)
	{
	}*/
	void UIOverlay::PrepareResources()
	{
		Application& app = Application::Get();
		const vk::Device& device = app.GetDevice();

		m_allocator.reset(new vk::Allocator(device, app.GetVulkanInstance(), "UI Allocator"));

		m_cmdPool = vk::CommandPool::Builder(device)
			.SetTag("UI CmdPool")
			.SetFlags(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT)
			.SetQFamily(device.GetGraphicsQ().GetFamily())
			.Build();

		m_descPool = vk::DescriptorPool::Builder(device)
			.SetTag("UI Descriptor Pool")
			.SetMaxSets(3)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3)
			.Build();



		ImGuiIO& io = ImGui::GetIO();

		// Create font texture
		unsigned char* fontData;
		uint32_t texWidth, texHeight;

		/*const std::string filename = getAssetPath() + "Roboto-Medium.ttf";
		io.Fonts->AddFontFromFileTTF(filename.c_str(), 16.0f * scale);*/
		
		io.Fonts->GetTexDataAsRGBA32(&fontData, (int*)&texWidth, (int*) & texHeight);
		VkDeviceSize uploadSize = texWidth * texHeight * 4 * sizeof(char);

		//SRS - Set ImGui style scale factor to handle retina and other HiDPI displays (same as font scaling above)
		ImGuiStyle& style = ImGui::GetStyle();
		style.ScaleAllSizes(scale);

		// Create target image for copy
		m_fontImage = vk::Image::Builder(device, *m_allocator)
			.SetTag("UI Font Image")
			.SetImgType(VK_IMAGE_TYPE_2D)
			.SetImageFormat(VK_FORMAT_R8G8B8A8_UNORM)
			.SetImageTiling(VK_IMAGE_TILING_OPTIMAL)
			.SetUsageFlags(VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT)
			.SetSharingMode(VK_SHARING_MODE_EXCLUSIVE)
			.SetImgExtent(VkExtent3D{ texWidth, texHeight,1 })
			.Build();
		m_fontImageView = vk::ImageView::Builder(device, *m_fontImage)
			.SetTag("UI Font ImageView")
			.SetViewType(VK_IMAGE_VIEW_TYPE_2D)
			.SetSubresourceAspectFlags(VK_IMAGE_ASPECT_COLOR_BIT)
			.SetFormat(m_fontImage->GetFormat())
			.Build();
		m_fontSampler = vk::Sampler::Builder(device)
			.SetTag("UI Font Sampler")
			.SetFilter(VK_FILTER_LINEAR)
			.SetMipmapMode(VK_SAMPLER_MIPMAP_MODE_LINEAR)
			.SetAddressMode(VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE)
			.Build();

		/// Copy buffer data to font image
		m_fontImage->CopyImageFromData(fontData, uploadSize, device.GetGraphicsQ(), *m_cmdPool);
		
		// Descriptor pool
		m_descPool = vk::DescriptorPool::Builder(device)
			.SetTag("UI descPool")
			.SetMaxSets(3)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1)
			.Build();
		// Descriptor set layout
		m_descSetLayout = vk::DescriptorSetLayout::Builder(device)
			.AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.SetTag("UI Descriptor Set Layout")
			.Build();
		// Descriptor set
		m_descriptorSet = vk::DescriptorSet::Allocator(device, *m_descPool)
			.SetTag("UI font DescriptorSet")
			.SetDescSetLayout(*m_descSetLayout)
			.Allocate();
		m_descriptorSet->
			BindDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, *m_fontSampler , *m_fontImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
			.UpdateDescriptors();
	}

	void UIOverlay::PreparePipeline()
	{
		const vk::Device& device = Application::Get().GetDevice();
		
		m_pipelineLayout = vk::PipelineLayout::Builder(device)
			.SetTag("UI pipeline layout")
			.AddPushConstant(sizeof(PushConstBlock))
			.BindDescriptorSetLayout(*m_descSetLayout)
			.Build();

		m_pipeline = vk::GraphicsPipeline::Builder(device, *m_pipelineLayout, m_renderPass, *m_pipelineCache)
			.SetTag("UI graphics pipeline")
			.AddPipelineStage(*m_vertexShader)
			.AddPipelineStage(*m_fragShader)
			.SetVertexProcessingState(false)
			.SetTessellationState()
			.SetRasterizationState(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE, VK_FALSE)
			//.SetDepthState(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL)
			.SetMultisampleState(VK_SAMPLE_COUNT_1_BIT)
			.AddColorBlendAttachmentState(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT, VK_TRUE)
			.SetColorBlendState(1)
			.SetViewportState(1, 1)
			.SetDynamicState(VK_FALSE)
			.Build();
	}


}
