#pragma once
#include "Base.h"
#include "Events/Event.h"
#include <string>

#include <GLFW/glfw3.h>
// struct GLFWwindow;
namespace Alalba
{
	class WindowProps
	{
	public:
		std::string Title;
		unsigned int Width;
		unsigned int Height;

		WindowProps(const std::string& title = "Alalba Engine",
			unsigned int width = 1280,
			unsigned int height = 720)
			: Title(title), Width(width), Height(height)
		{

		}
	};

	struct InstanceExtentions_GLFW
	{
		std::vector<const char*> names;
		unsigned int count;
	};

	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;
		Window(const WindowProps& props = WindowProps());
		virtual ~Window();
		void OnUpdate();
		inline unsigned int GetWidth()const { return m_Data.Width; };
		inline unsigned int GetHeight() const { return m_Data.Height; };
		
		//Window Attribute
		inline void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback; };
		void SetVSync(bool enabled);
		bool IsVSync()const;
		inline virtual void* GetNativeWindow() const { return m_Window; };
		
		// Vulkan related
		std::vector<const char*> GetRequiredInstanceExtensions() const;
	private:
		void Init(const WindowProps& props);
		void Shutdown();
	private:
		GLFWwindow* m_Window;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;

			bool VSync;
			EventCallbackFn EventCallback;
		};
		WindowData m_Data;
	};
}
