#pragma once
#include "Vertex.h"



namespace vk
{
	class Buffer;
}

namespace Alalba
{
	class Geometry
	{
	public:
		static Geometry* Create(const std::string file);
		static Geometry* Create() {};

		virtual ~Geometry() {}

		virtual void Clean() = 0;

		//TODO:  MOVE TO  the mesh
		virtual const vk::Buffer& GetVertexbuffer() const = 0;
		virtual const vk::Buffer& GetIndexbuffer() const = 0;
		virtual const uint32_t GetIndexCount() const = 0;
		virtual const uint32_t GetInstanceCount() const = 0;

		virtual const glm::mat4 ModelMatrix() const = 0;
		virtual Geometry& SetModelMatirx(glm::mat4 model) = 0;

	};
}


