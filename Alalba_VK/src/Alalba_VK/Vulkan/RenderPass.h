#pragma once
#include <vulkan/vulkan.h>
// TODO: should have a add attachment function
//https://stackoverflow.com/questions/66695769/vulkan-loading-depth-attachment-in-the-second-renderpass-not-working#:~:text=void%20VulkanRenderTarget%3A%3AaddAttachment%20%28AttachmentCreateInfo%20createinfo%29%20%7B%20auto%20device%20%3D,VulkanInitializers%3A%3AimageViewCreateInfo%20%28%29%3B%20imageView.viewType%20%3D%20%28createinfo.layerCount%20%3D%3D%201%29%20%3F
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
			Builder& PushAttachment(){}
			Builder& SetTag(const std::string tag) { m_tag = tag; return *this; }
			std::unique_ptr<RenderPass> Build() const;

		private:
			const class Device& m_device;
			VkAttachmentLoadOp m_colorATCHLoadOp{};
			VkAttachmentLoadOp m_depthATCHLoadOp{};
			VkFormat m_colorFormat{};
			VkFormat m_depthFormat{};
			std::string m_tag;
		};

	public:
		VULKAN_NON_COPIABLE(RenderPass);
		RenderPass(const Device& device, 
			const VkFormat colorForamt, const VkFormat depthFormat,
			const VkAttachmentLoadOp colorATCHLoadOp,
			const VkAttachmentLoadOp depthATCHLoadOp,
			const std::string tag
		);
		~RenderPass() { Clean(); };
		void Clean();

	private:
		VULKAN_HANDLE(VkRenderPass, m_renderPass);
		const class Device& m_device;
	};
}
