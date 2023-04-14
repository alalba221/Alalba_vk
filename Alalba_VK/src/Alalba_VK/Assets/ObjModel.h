#pragma once
#include "Alalba_VK/Assets/Mesh.h"
#include "Alalba_VK/Assets/Texture.h"
#include "Alalba_VK/Vulkan/DescriptorPool.h"
#include "Alalba_VK/Vulkan/DescriptorSet.h"
#include "Alalba_VK/Vulkan/DescriptorSetLayout.h"
#include "Alalba_VK/Vulkan/Buffer.h"
#include <glm/glm.hpp>


namespace Alalba
{
	class ObjModel
	{
	public:
		ObjModel(const Mesh& mesh, const Texture& texture, const Scene& scene);
		~ObjModel() { Clean(); };
		void Clean();
		static inline vk::DescriptorSetLayout* GetDescLayout() { return s_descLayout; }
		const vk::DescriptorSet& GetDescSet() const { return *m_descSet.get(); }

		const Mesh& GetMesh()const { return m_mesh; }
	private:
		static vk::DescriptorSetLayout* s_descLayout;
		const class Texture& m_texture;
		const class Mesh& m_mesh;
		//std::unique_ptr<vk::DescriptorPool> m_descriptorPool;
		
		// TODO: add materials buffers
		// std::unique_ptr<vk::Buffer> m_materialBuffer;
		std::unique_ptr<vk::DescriptorSet> m_descSet; // binding 0: texture; 1....: others
		
		// Transformation
		glm::vec3 m_modelMatrix;
	};
}


