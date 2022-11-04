#include "pch.h"
#include "Surface.h"
#include <GLFW/glfw3.h>

#include "Instance.h"
#include "Alalba_VK/Core/Window.h"
namespace vk
{
	std::unique_ptr<Surface> Surface::Builder::Build()
	{
		return std::make_unique<Surface>(m_instance, m_window);
	}

	Surface::Surface(const Instance& instance, const Alalba::Window& window)
		:m_instance(instance), m_window(window)
	{
		VkResult err;
		err = glfwCreateWindowSurface(m_instance.Handle(), (GLFWwindow*)m_window.GetNativeWindow(),nullptr, &m_surface);
		ALALBA_ASSERT(err == VK_SUCCESS, "Create glfw surface failed");
	}
	Surface::~Surface()
	{
		Clean();
	}

	void Surface::Clean()
	{
		if (m_surface != VK_NULL_HANDLE)
		{
			ALALBA_WARN("Clean GLFW Surface {0}", m_tag);
			vkDestroySurfaceKHR(m_instance.Handle(), m_surface, nullptr);
			m_surface = VK_NULL_HANDLE;
		}
	}

	const Surface::SupportDetails Surface::FindDetails() const
	{
		Surface::SupportDetails details;
	
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_instance.GetPhysicalDevice().Handle(), m_surface, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_instance.GetPhysicalDevice().Handle(), m_surface, &formatCount, nullptr);
		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(m_instance.GetPhysicalDevice().Handle(), m_surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(m_instance.GetPhysicalDevice().Handle(), m_surface, &presentModeCount, nullptr);
		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(m_instance.GetPhysicalDevice().Handle(), m_surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}
}