#include "pch.h"
#include "Application.h"
// included before event sys
#include <GLFW/glfw3.h>

#include "Events/ApplicationEvent.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <Windows.h>
//

namespace Alalba
{
	#define BIND_ENVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;
	Application::Application()
	{
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(new Window());
		m_Window->SetEventCallback(BIND_ENVENT_FN(Application::OnEvent));
		
		
		{
			m_vulkanInstance = vk::Instance::Builder(m_Window.get())
				.SetVulkanVersino(VK_API_VERSION_1_3)
#ifdef  ALALBA_DEBUG
				.RequestLayer("VK_LAYER_KHRONOS_validation")
#endif //  ALALBA_DEBUG
				.RequestLayer("VK_LAYER_LUNARG_monitor")
				.Build();

			m_vulkanSurface = vk::Surface::Builder(*m_vulkanInstance, *m_Window)
				.Build();

			m_vulkanInstance->SelectPhysicalDevice();

			m_vulkanDevice = vk::Device::Builder(m_vulkanInstance->GetPhysicalDevice())
				.AddExtension("VK_KHR_swapchain")
				.AddExtension("VK_KHR_ray_tracing_pipeline")
				.AddExtension("VK_KHR_acceleration_structure")
				.AddExtension("VK_KHR_deferred_host_operations")
				.AddExtension("VK_KHR_ray_query")
				.AddExtension("VK_KHR_buffer_device_address")
				// for debug computer shader
				.AddExtension("VK_KHR_shader_non_semantic_info")
				.Build();
		}


		m_startTimePoint = std::chrono::steady_clock::now();
	}

	void Application::OnInit()
	{
		
	}

	void Application::OnShutdown()
	{				
		//m_ui->Clean();
		m_vulkanDevice->Clean();
		m_vulkanSurface->Clean();
		m_vulkanInstance->Clean();
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		OnInit();

		m_lastTimePoint = std::chrono::steady_clock::now();
		while(m_Running)
		{
			float deltaTime = std::chrono::duration<float>(std::chrono::steady_clock::now() - m_lastTimePoint).count();
			m_lastTimePoint = std::chrono::steady_clock::now();
			
			OnUpdate(deltaTime);
			m_Window->OnUpdate();
			m_Window->SwapBuffer();
		}
		OnShutdown();
	}

	void Application::OnEvent(Event& event)
	{
		// By now  only one event, windows close
		//
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowResizeEvent>(BIND_ENVENT_FN(Application::OnWindowResize));
		dispatcher.Dispatch<WindowCloseEvent>(BIND_ENVENT_FN(Application::OnWindowClose));
	}
	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		//LOG_ERROR("{0}", e);
		//vk::VulkanRenderer::Get(*m_vulkanDevice, *m_allocator).Resize();
		return false;
	}
}