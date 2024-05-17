#pragma once
#include <vulkan/vulkan.h>
namespace vk
{
	class Device;
	class Queue;
	class CommandPool;
	class CommandBuffers
	{
	public:
		class Allocator
		{
		public: 
			Allocator(const Device& device, const CommandPool& cmdPool) :m_device(device), m_cmdPool(cmdPool) {};
			Allocator& SetTag(const std::string& tag) { m_tag = tag; return *this; }
			Allocator& SetSize(uint32_t count) { m_count = count; return *this; }
			Allocator& OneTimeSubmit(bool onetime) { m_oneTimeSubmit = onetime; return *this; }

			std::unique_ptr<CommandBuffers> Allocate() const 
			{
				return std::make_unique<CommandBuffers>(m_device,m_cmdPool,m_count,m_oneTimeSubmit,m_tag);
			}
		private:
			const class Device& m_device;
			const class CommandPool& m_cmdPool;
			uint32_t m_count = 1;
			std::string m_tag;
			bool m_oneTimeSubmit = false;
		};

	public:
		VULKAN_NON_COPIABLE(CommandBuffers);
		CommandBuffers(const Device& device, const CommandPool& cmdPool, const uint32_t count, bool oneTimeSubmit, const std::string& tag);
		VkCommandBuffer operator [] (const size_t i) const { return m_cmdBuffers[i]; }

		~CommandBuffers() { Clean(); };
		void BeginRecording(uint32_t index);
		void EndRecording(uint32_t index);
		// for now only used by one-time command
		void Flush(uint32_t index, const Queue& q);

		void Clean();

	private:
		//VULKAN_HANDLE(VkCommandBuffer, m_cmdBuffer);
		std::vector<VkCommandBuffer> m_cmdBuffers;
		std::string m_tag;
		bool m_oneTimeSubmit;
		const class Device& m_device;
		const class CommandPool& m_cmdPool;
	
	};

	typedef std::unique_ptr<CommandBuffers> CommandBufferPtrs;
}


