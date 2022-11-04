#pragma once
#include <vulkan/vulkan.h>

namespace vk
{
	class Device;
	class RenderPass
	{
	public: 
		class Builder 
		{
		public:
			Builder(const Device& device):m_device(device) {};
			Builder& SetColorFormat(const VkFormat colorFormat);
			Builder& SetDepthFormat(const VkFormat depthFormat);
			Builder& SetColorATCHLoadOP(const VkAttachmentLoadOp colorATCHLoadOp);
			Builder& SetDepthATCHLoadOP(const VkAttachmentLoadOp depthATCHLoadOp);
			std::unique_ptr<RenderPass> Build() const;

		private:
			const class Device& m_device;
			VkAttachmentLoadOp m_colorATCHLoadOp{};
			VkAttachmentLoadOp m_depthATCHLoadOp{};
			VkFormat m_colorFormat{};
			VkFormat m_depthFormat{};

		};

	public:
		VULKAN_NON_COPIABLE(RenderPass);
		RenderPass(const Device& device, 
			const VkFormat colorForamt, const VkFormat depthFormat,
			const VkAttachmentLoadOp colorATCHLoadOp,
			const VkAttachmentLoadOp depthATCHLoadOp
		);
		~RenderPass() { Clean(); };
		void Clean();

	private:
		VULKAN_HANDLE(VkRenderPass, m_renderPass);
		const class Device& m_device;
	};
}
