#include "pch.h"
#include "UI.h"
#include "Alalba_VK/Vulkan/DescriptorPool.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"


#include "Alalba_VK/Vulkan/CommandBuffers.h"
namespace Alalba
{
	
	UI::UI(const vk::VulkanRenderer& renderer, const Window& window)
		:m_renderer(renderer)
	{
		// the size of the pool is very oversize, but it's copied from imgui demo itself.
		m_descriptorPool = vk::DescriptorPool::Builder(Application::Get().GetDevice())
			.SetTag("Descriptor Pool")
			.SetMaxSets(11*1000)
			.AddPoolSize( VK_DESCRIPTOR_TYPE_SAMPLER, 1000 )
			.AddPoolSize( VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 )
			.AddPoolSize( VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 )
			.AddPoolSize( VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 )
			.AddPoolSize( VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 )
			.AddPoolSize( VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 )
			.AddPoolSize( VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 )
			.AddPoolSize( VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 )
			.AddPoolSize( VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 )
			.AddPoolSize( VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 )
			.AddPoolSize( VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 )
			.Build();

		//  specific render pass dedicated to Dear ImGui
		m_renderPass = vk::RenderPass::Builder(Application::Get().GetDevice())
			.SetTag("Imgui renderpass")
			.SetColorFormat(m_renderer.GetSwapChain().GetFormat())
			.SetDepthFormat(m_renderer.GetDepthImage().GetFormat()) // this should be compatible with framebuffer
			.SetColorATCHLoadOP(VK_ATTACHMENT_LOAD_OP_LOAD)
			.SetDepthATCHLoadOP(VK_ATTACHMENT_LOAD_OP_LOAD)
			.Build();

		// Initialise ImGui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();
		
		// Initialise ImGui GLFW adapter
		ImGui_ImplGlfw_InitForVulkan((GLFWwindow*)window.GetNativeWindow(), true);


		// Initialise ImGui Vulkan adapter
		ImGui_ImplVulkan_InitInfo vulkanInit = {};
		vulkanInit.Instance = Application::Get().GetVulkanInstance().Handle();
		vulkanInit.PhysicalDevice = Application::Get().GetVulkanInstance().GetPhysicalDevice().Handle();
		vulkanInit.Device = Application::Get().GetDevice().Handle();
		vulkanInit.QueueFamily = Application::Get().GetVulkanInstance().GetPhysicalDevice().GetQFamilies().graphics.value();
		vulkanInit.Queue = Application::Get().GetDevice().GetGraphicsQ().Handle();
		vulkanInit.PipelineCache = m_renderer.GetPipelineCache().Handle();
		vulkanInit.DescriptorPool = m_descriptorPool->Handle();
		vulkanInit.MinImageCount = m_renderer.GetSwapChain().GetImgCount();
		vulkanInit.ImageCount = m_renderer.GetSwapChain().GetImgCount();
		vulkanInit.Allocator = nullptr;
		//vulkanInit.CheckVkResultFn = CheckVulkanResultCallback;

		if (!ImGui_ImplVulkan_Init(&vulkanInit, m_renderPass->Handle()))
		{
			ALALBA_ASSERT("Failed to initialise ImGui vulkan adapter");
		}

		//Execute a gpu command to upload imgui font textures
		std::unique_ptr<vk::CommandBuffers>CmdBuffer = vk::CommandBuffers::Allocator(
			Application::Get().GetDevice(), renderer.GetCommandPool())
			.SetTag("Command Buffers for ImGui font")
			.OneTimeSubmit(true)
			.SetSize(1)
			.Allocate();

		CmdBuffer->BeginRecording(0);
		VkCommandBuffer cmdbuffer = (*CmdBuffer.get())[0];
		ImGui_ImplVulkan_CreateFontsTexture(cmdbuffer);
		CmdBuffer->EndRecording(0);

		CmdBuffer->Flush(0, Application::Get().GetDevice().GetGraphicsQ());
		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}
	void UI::RenderCommand(const uint32_t frameBufferIndex) const
	{
		//// Start the Dear ImGui frame
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_renderPass->Handle();
		renderPassInfo.framebuffer = m_renderer.GetFramebuffer(frameBufferIndex).Handle();
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = m_renderer.GetSwapChain().GetExtent();
		renderPassInfo.clearValueCount = 0;
		renderPassInfo.pClearValues = nullptr;

		
		ImGui::ShowDemoWindow();
		ImGui::Render();

		// vkCmdBeginRenderPass(m_renderer.GetCommandBuffers()[frameBufferIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_renderer.GetCommandBuffers()[frameBufferIndex]);
		// vkCmdEndRenderPass(m_renderer.GetCommandBuffers()[frameBufferIndex]);
	}
	void UI::Clean()
	{
		m_renderPass->Clean();
		m_descriptorPool->Clean();
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
}