#pragma once
#include "Vertex.h"
namespace vk
{
	class Buffer;
	class Allocator;
	class CommandPool;

}

namespace Alalba
{
	class Model
	{
	public:
		static Model* Create(const std::string file);
		static Model* Create() {};

		virtual ~Model() {}

		virtual void Clean() = 0;

		//TODO:  MOVE TO  the mesh
		virtual const vk::Buffer& GetVertexbuffer() const = 0;
		virtual const vk::Buffer& GetIndexbuffer() const = 0;
		virtual const uint32_t GetIndexCount() const = 0;
		virtual const uint32_t GetInstanceCount() const = 0;

		virtual const glm::mat4 ModelMatrix() const = 0;
		virtual Model & SetModelMatirx(glm::mat4 model) = 0;


		static vk::Allocator* Allocator() { return s_allocator; }
		static vk::CommandPool* CommandPool() { return s_commandPool; }

		// Allocator for Mesh
		static vk::Allocator* s_allocator;
		static vk::CommandPool* s_commandPool;

		
	};
}


