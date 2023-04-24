#pragma once
#include "glm/glm.hpp"
#include "Alalba_VK/Assets/Mesh.h"
#include "Alalba_VK//Assets/Texture.h"
#include "Alalba_VK/Core/Camera.h"
#include "Alalba_VK/Vulkan/DescriptorSet.h"
namespace Alalba
{
	struct TransformComponent
	{
		glm::mat4 Transform = glm::mat4(1.0f);

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::mat4& transform)
			:Transform(transform){}

		operator glm::mat4& () { return Transform; }
		operator const glm::mat4& () const { return Transform; }

	};
	struct TagComponent
	{
		std::string Tag;
		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}
	};

	struct MeshComponent
	{
		std::shared_ptr<Mesh> m_Mesh;
		MeshComponent() = default;
		MeshComponent(const MeshComponent&) = default;
		MeshComponent(std::shared_ptr<Mesh> mesh)
			:m_Mesh(mesh)
		{}
	};

	struct TextureComponent
	{
		std::shared_ptr<Texture> m_Texture;
		TextureComponent() = default;
		TextureComponent(const TextureComponent&) = default;
		TextureComponent(std::shared_ptr<Texture> texture)
			:m_Texture(texture)
		{}
	};

	struct CamComponent
	{
		Camera m_Camera;
		CamComponent() = default;
		CamComponent(const CamComponent&) = default;
		CamComponent(const glm::mat4& projectionMatrix)
			:m_Camera(projectionMatrix)
		{}
	};
	struct DescrpotrSetCompont
	{
		std::shared_ptr<vk::DescriptorSet> DescriptorSet;
		DescrpotrSetCompont() = default;
		DescrpotrSetCompont(const DescrpotrSetCompont&) = default;
		DescrpotrSetCompont(std::shared_ptr<vk::DescriptorSet> set)
			:DescriptorSet(set)
		{}
	};
}