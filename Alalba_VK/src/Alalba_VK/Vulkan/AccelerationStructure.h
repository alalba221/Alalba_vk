#pragma once
#include <vulkan/vulkan.h>
#include "glm/glm.hpp"
namespace vk
{
	class Device;
	class Allocator;
	class CommandPool;
	class Queue;
	class Buffer;

	/// <summary>
	/// BLAS
	/// </summary>
	class BLAS
	{
	public:
		class Builder
		{
		public:
			Builder(const Device& device, Allocator& allocator, const Buffer& vertexBuffer, const Buffer& indexBuffer) 
				: m_device(device), m_allocator(allocator), m_vertexBuffer(vertexBuffer), m_indexBuffer(indexBuffer) {};
			Builder& SetTag(const std::string& tag) { m_tag = tag; return *this; }
			Builder& SetIndexCount(uint32_t indexcount) { m_indexCount = indexcount; return *this; }
			Builder& SetVertexCount(uint32_t vertexcount) { m_vertexCount = vertexcount; return *this; }
			
			std::unique_ptr<BLAS> Build() const
			{
				return std::make_unique<BLAS>(m_device, m_vertexBuffer, m_indexBuffer, m_vertexCount, m_indexCount, m_allocator, m_tag);
			};

		private:
			const class Device& m_device;
			class Allocator& m_allocator;
			const Buffer& m_vertexBuffer;
			const Buffer& m_indexBuffer;
			std::string m_tag;

			uint32_t m_vertexCount;
			uint32_t m_indexCount;
		};


	public:
		VULKAN_NON_COPIABLE(BLAS);
		BLAS(const Device& device, const Buffer& vertexBuffer, const Buffer& indexBuffer, 
			const uint32_t vertexCount, const uint32_t indexCount, Allocator& allocator, const std::string& tag);
		~BLAS() { Clean(); };
		void BuildOnDevice(const Queue& q, const CommandPool& cmdPool);
		void Clean();

	private:
		VULKAN_HANDLE(VkAccelerationStructureKHR, m_BLAS);
		const class Device& m_device;
		class Allocator& m_allocator;

		std::unique_ptr<Buffer> m_buffer;
		uint64_t m_deviceAddress;
		VkDeviceSize m_asSize;
		VkAccelerationStructureGeometryKHR  m_accelerationStructureGeometry{};
		uint32_t m_primitiveCount;

		friend class TLAS;
	};


	/// <summary>
	/// TLAS
	/// </summary>
 class TLAS
	{

	public:
		class Builder
		{
		public:
			Builder(const Device& device, Allocator& allocator)
				: m_device(device), m_allocator(allocator){};
			Builder& SetTag(const std::string& tag) { m_tag = tag; return *this; }

			Builder& AddInstance(const BLAS& blas, const glm::mat4& transform, uint32_t id, uint32_t sbtOffset);

			std::unique_ptr<TLAS> Build(const Queue& q, const CommandPool& cmdPool) const
			{
				return std::make_unique<TLAS>(m_device, m_allocator, m_instances, q, cmdPool);
			};

		private:
			const class Device& m_device;
			class Allocator& m_allocator;
			std::string m_tag;
			
			std::vector<VkAccelerationStructureInstanceKHR>m_instances;
		};
	public:
		VULKAN_NON_COPIABLE(TLAS);
		TLAS(const Device& device, Allocator& allocator, const std::vector<VkAccelerationStructureInstanceKHR>& instances, const Queue& q, const CommandPool& cmdPool);
		

		~TLAS() { Clean(); };
		//void BuildOnDevice(const Queue& q, const CommandPool& cmdPool);
		void Clean();

	private:
		VULKAN_HANDLE(VkAccelerationStructureKHR, m_TLAS);
		const class Device& m_device;
		class Allocator& m_allocator;
		std::unique_ptr<Buffer> m_buffer;
		uint64_t m_deviceAddress;
		std::vector<VkAccelerationStructureInstanceKHR> m_instances;

 };
}


