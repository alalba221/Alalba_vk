#pragma once
#include <vulkan/vulkan.h>
#include "VkCommon.h"
namespace Alalba
{
	class Window;
}
namespace vk
{
	// create from
	class Instance;

	class Surface final
	{
	public:
		class Builder
		{
		public:
			Builder(const Instance& instance, const Alalba::Window& window)
				:m_instance(instance), m_window(window) {};
			std::unique_ptr<Surface> Build();

		private:
			const class Instance& m_instance;
			const class Alalba::Window& m_window;
		};

	public:
		VULKAN_NON_COPIABLE(Surface);
		Surface(const Instance& instance, const Alalba::Window& window);
		~Surface();
		void Clean();

		struct SupportDetails {
			VkSurfaceCapabilitiesKHR capabilities;
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> presentModes;
		};

		const SupportDetails FindDetails()const;

		const Alalba::Window& GetWindow() const { return m_window;}
	private:
		VULKAN_HANDLE(VkSurfaceKHR, m_surface);

		const class Instance& m_instance;
		const class Alalba::Window& m_window;
	};
}
