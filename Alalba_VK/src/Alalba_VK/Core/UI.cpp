#include "pch.h"
#include "UI.h"
#include "Alalba_VK/Vulkan/DescriptorPool.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

#include "Application.h"
#include "Alalba_VK/Vulkan/CommandBuffers.h"
namespace Alalba
{
	void CheckVulkanResultCallback(const VkResult err)
	{
		if (err != VK_SUCCESS)
		{
			ALALBA_ASSERT("ImGui Vulkan error ");
		}
	}

	UI::UI(const vk::VulkanRenderer& renderer, const Window& window)
	{
		// the size of the pool is very oversize, but it's copied from imgui demo itself.
		m_descriptorPool = vk::DescriptorPool::Builder(renderer.GetDevice())
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

		// Initialise ImGui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		
		// Initialise ImGui GLFW adapter
		if (!ImGui_ImplGlfw_InitForVulkan((GLFWwindow*)window.GetNativeWindow(), true))
		{
			ALALBA_ASSERT("Failed to initialise ImGui GLFW adapter");
		}

		// Initialise ImGui Vulkan adapter
		ImGui_ImplVulkan_InitInfo vulkanInit = {};
		vulkanInit.Instance = Application::Get().GetVulkanInstance().Handle();
		vulkanInit.PhysicalDevice = Application::Get().GetVulkanInstance().GetPhysicalDevice().Handle();
		vulkanInit.Device = Application::Get().GetDevice().Handle();
		vulkanInit.QueueFamily = Application::Get().GetVulkanInstance().GetPhysicalDevice().GetQFamilies().graphics.value();
		vulkanInit.Queue = Application::Get().GetDevice().GetGraphicsQ().Handle();
		vulkanInit.PipelineCache = nullptr;
		vulkanInit.DescriptorPool = m_descriptorPool->Handle();
		vulkanInit.MinImageCount = renderer.GetSwapChain().GetImgCount();
		vulkanInit.ImageCount = renderer.GetSwapChain().GetImgCount();
		vulkanInit.Allocator = nullptr;
		vulkanInit.CheckVkResultFn = CheckVulkanResultCallback;

		if (!ImGui_ImplVulkan_Init(&vulkanInit, renderer.GetRenderPass().Handle()))
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
	
	}
	void UI::Draw()
	{
		//// Start the Dear ImGui frame
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		
		ImGui::ShowDemoWindow();
		ImGui::Render();


	}
}