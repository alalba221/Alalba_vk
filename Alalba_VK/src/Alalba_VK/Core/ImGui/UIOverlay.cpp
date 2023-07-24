#include "pch.h"
#include "UIOverlay.h"
#include "imgui.h"
#include "Alalba_VK/Vulkan/RenderPass.h"
#include "Alalba_VK/Core/Application.h"
#include "Alalba_VK/Vulkan/CommandBuffers.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

namespace Alalba
{

	UIOverlay::UIOverlay(const vk::RenderPass& renderPass)
		:m_renderPass(renderPass)
	{
		Application& app = Application::Get();
		const vk::Device& device = Application::Get().GetDevice();
		
		//m_allocator.reset(new vk::Allocator(device, app.GetVulkanInstance(), "UI Allocator"));

		m_cmdPool = vk::CommandPool::Builder(device)
			.SetTag("UI CmdPool")
			.SetFlags(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT)
			.SetQFamily(device.GetGraphicsQ().GetFamily())
			.Build();

		m_descPool = vk::DescriptorPool::Builder(device)
			.SetTag("UI Descriptor Pool")
			.SetMaxSets(11 * 1000)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_SAMPLER, 1000)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000)
			.Build();

		m_pipelineCache = vk::PipelineCache::Builder(device)
			.SetTag("UI PipelineCache")
			.Build();

		// Init ImGui
		ImGui::CreateContext();

		// Color scheme

		// Dimensions
		ImGuiIO& io = ImGui::GetIO();

		// Color scheme
		ImGui::StyleColorsDark();
		// Initialise ImGui GLFW adapter
		ImGui_ImplGlfw_InitForVulkan((GLFWwindow*)app.GetWindow().GetNativeWindow(), true);


		// Initialise ImGui Vulkan adapter
		ImGui_ImplVulkan_InitInfo vulkanInit = {};
		vulkanInit.Instance = Application::Get().GetVulkanInstance().Handle();
		vulkanInit.PhysicalDevice = Application::Get().GetVulkanInstance().GetPhysicalDevice().Handle();
		vulkanInit.Device = Application::Get().GetDevice().Handle();
		vulkanInit.QueueFamily = Application::Get().GetVulkanInstance().GetPhysicalDevice().GetQFamilies().graphics.value();
		vulkanInit.Queue = Application::Get().GetDevice().GetGraphicsQ().Handle();
		vulkanInit.PipelineCache = m_pipelineCache->Handle();
		vulkanInit.DescriptorPool = m_descPool->Handle();
		vulkanInit.MinImageCount =3;
		vulkanInit.ImageCount = 3;
		vulkanInit.Allocator = nullptr;
		//vulkanInit.CheckVkResultFn = CheckVulkanResultCallback;

		if (!ImGui_ImplVulkan_Init(&vulkanInit, m_renderPass.Handle()))
		{
			ALALBA_ASSERT("Failed to initialise ImGui vulkan adapter");
		}



		//Execute a gpu command to upload imgui font textures
		std::unique_ptr<vk::CommandBuffers>CmdBuffer = vk::CommandBuffers::Allocator(
			Application::Get().GetDevice(), *m_cmdPool)
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
	void UIOverlay::Clean()
	{
		m_descPool->Clean();
		m_cmdPool->Clean();
		m_pipelineCache->Clean();
		//m_allocator->Clean();
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
	UIOverlay::~UIOverlay()
	{
		Clean();
	}

	void UIOverlay::Draw(const vk::CommandBuffers& cmdbuffers, const uint32_t currentCmdBuffer)
	{
		ImDrawData* imDrawData = ImGui::GetDrawData();
		int32_t vertexOffset = 0;
		int32_t indexOffset = 0;

		if ((!imDrawData) || (imDrawData->CmdListsCount == 0)) {
			return;
		}
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdbuffers[currentCmdBuffer]);
	
	}
	
	/*void UIOverlay::Draw(const vk::CommandBuffers& cmdbuffers, const int currentCmdBuffer)
	{
	}*/
	

	void UIOverlay::NewFrame()
	{
		Application& app = Application::Get();

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
		//ImGui::SetNextWindowPos(ImVec2(10 , 10 ));
		//ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_FirstUseEver);
		//ImGui::Begin("Vulkan Example", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		////ImGui::TextUnformatted(title.c_str());
		//Application& app = Application::Get();
		////ImGui::TextUnformatted(app.GetDevice().);
		////ImGui::Text("%.2f ms/frame (%.1d fps)", (1000.0f / lastFPS), lastFPS);
		app.DesignUI(this);
		//ImGui::ShowDemoWindow();
		ImGui::Render();

	}
	bool UIOverlay::BufferUpdated()
	{
		ImDrawData* imDrawData = ImGui::GetDrawData();
		bool updateCmdBuffers = false;

		

		if (!imDrawData) { return false; };

		// Note: Alignment is done inside buffer creation
		VkDeviceSize vertexBufferSize = imDrawData->TotalVtxCount * sizeof(ImDrawVert);
		VkDeviceSize indexBufferSize = imDrawData->TotalIdxCount * sizeof(ImDrawIdx);

		// Update buffers only if vertex or index count has been changed compared to current buffer size
		if ((vertexBufferSize == 0) || (indexBufferSize == 0)) {
			return false;
		}

		if ((vertexCount != imDrawData->TotalVtxCount))
		{
			vertexCount = imDrawData->TotalVtxCount;
			updateCmdBuffers = true;
		}
			

		if ((indexCount < imDrawData->TotalIdxCount))
		{
			indexCount = imDrawData->TotalIdxCount;
			updateCmdBuffers = true;
		}

		return true;
		return updateCmdBuffers;
	}
}
