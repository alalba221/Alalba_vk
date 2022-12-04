#pragma once
#include "Window.h"
#include <memory>
//#include "Alalba_VK/Vulkan/VulkanContext.h" // need to be deleted
//#include "Alalba_VK/Assets/Scene.h"
#include "Camera.h"
#include "Events/ApplicationEvent.h"

#include "Alalba_VK/Vulkan/Instance.h"
#include "Alalba_VK/Vulkan/Device.h"
#include "Alalba_VK/Vulkan/Surface.h"
#include "Alalba_VK/Vulkan/Allocator.h"

#include "Alalba_VK/Vulkan/CommandPool.h"
#include "Alalba_VK/Vulkan/CommandBuffers.h"

#include "Alalba_VK/Vulkan/VulkanRenderer.h"
#include "Alalba_VK/Assets/Model.h"
#include "Alalba_VK/Assets/Texture.h"

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

		virtual void OnEvent(Event& event);

		static inline Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window; }
	
		const vk::Surface& GetSurface() const { return *m_vulkanSurface.get(); };

		const vk::Instance& GetVulkanInstance() const { return *m_vulkanInstance.get(); }
		const vk::Device& GetDevice() const { return *m_vulkanDevice.get(); }
		
		const vk::VulkanRenderer& GetRenderer() const { return *m_renderer.get(); }

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;

		static Application* s_Instance;
	protected:
		std::unique_ptr<vk::Instance> m_vulkanInstance;
		std::unique_ptr<vk::Surface> m_vulkanSurface;
		std::unique_ptr<vk::Device> m_vulkanDevice;
		std::unique_ptr<vk::VulkanRenderer> m_renderer;
		
		// TODO: combine mesh texture material into a Scene class
		std::unique_ptr<Mesh> m_mesh;
		std::unique_ptr<Texture> m_testTexture;

		//Ref<Scene> m_Scene;
		Ref<Camera> m_Camera;
	};

	// to be defined in sandbox
	Application* CreateApplication();
}
