#pragma once
#include "Window.h"
#include <memory>

#include "Camera.h"
#include "Events/ApplicationEvent.h"

#include "Alalba_VK/Vulkan/Instance.h"
#include "Alalba_VK/Vulkan/Device.h"
#include "Alalba_VK/Vulkan/Surface.h"

#include "Alalba_VK/Core/ImGui/UIOverlay.h"

namespace Alalba{

	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		virtual void OnInit();
		virtual void OnUpdate() {}
		virtual void OnShutdown();

		virtual void DesignUI(UIOverlay* overlay) {};

		virtual void OnEvent(Event& event);

		static inline Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window; }
	
		const vk::Surface& GetSurface() const { return *m_vulkanSurface; };

		const vk::Instance& GetVulkanInstance() const { return *m_vulkanInstance; }
		const vk::Device& GetDevice() const { return *m_vulkanDevice; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;

		static Application* s_Instance;
		std::unique_ptr<vk::Instance> m_vulkanInstance;
		std::unique_ptr<vk::Surface> m_vulkanSurface;
		std::unique_ptr<vk::Device> m_vulkanDevice;
	
	protected:
	
	public:
		//UIOverlay* m_ui;
	};

	// to be defined in sandbox
	Application* CreateApplication();
}
