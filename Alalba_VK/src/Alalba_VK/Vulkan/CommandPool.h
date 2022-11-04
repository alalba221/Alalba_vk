#pragma once
#include <vulkan/vulkan.h>

namespace vk
{
	class Device;
	class CommandPool
	{
	public:
		class Builder 
		{
		public:

			Builder(const Device& device) :m_device(device) {};
			Builder& SetTag(const std::string& tag) { m_tag = tag; return *this; }
			Builder& SetQFamily(const uint32_t QFamily) { m_QFamily = QFamily; return *this; }
			// VK_COMMAND_POOL_CREATE_TRANSIENT_BIT = 0x00000001,
			// VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT = 0x00000002,
			// // Provided by VK_VERSION_1_1
			// VK_COMMAND_POOL_CREATE_PROTECTED_BIT = 0x00000004,

			Builder& SetFlags(const VkCommandPoolCreateFlags flags) { m_flags = flags; return *this; }
			std::unique_ptr<CommandPool>Build() const
			{
				return std::make_unique<CommandPool>(m_device, m_QFamily, m_flags, m_tag);
			};

		private:
			const class Device& m_device;
			std::string m_tag;
			uint32_t m_QFamily;
			VkCommandPoolCreateFlags m_flags;
		};

	public:
		VULKAN_NON_COPIABLE(CommandPool);
		CommandPool(const Device& device, const uint32_t QFamily, const VkCommandPoolCreateFlags flags, const std::string& tag);
		~CommandPool() { Clean(); }
		void Clean();

	private:
		VULKAN_HANDLE(VkCommandPool, m_cmdPool);
		const class Device& m_device;
		/*All command buffers allocated from this command pool must be submitted on queues from the same queue family.*/
		uint32_t m_QFamily;
		VkCommandPoolCreateFlags m_flags;
	};
}


