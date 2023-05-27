#pragma once

#include "Vertex.h"
#include "Alalba_VK/Vulkan/Buffer.h"
#include "Alalba_VK/Vulkan/DescriptorSet.h"
#include "Alalba_VK/Vulkan/DescriptorSetLayout.h"
#include "Texture.h"
#include "Alalba_VK/Vulkan/Image.h"
#include "Alalba_VK/Vulkan/CommandBuffers.h"
#include "tiny_gltf.h"

namespace Alalba
{
	enum class FileLoadingFlags 
	{
		None = 0x00000000,
		PreTransformVertices = 0x00000001,
		PreMultiplyVertexColors = 0x00000002,
		FlipY = 0x00000004,
		DontLoadImages = 0x00000008
	};

	enum class RenderFlags {
		BindImages = 0x00000001,
		RenderOpaqueNodes = 0x00000002,
		RenderAlphaMaskedNodes = 0x00000004,
		RenderAlphaBlendedNodes = 0x00000008
	};

	class GLTFLoader;

	// A glTF material stores information in e.g. the texture that is attached to it and colors
	struct Material 
	{
		enum AlphaMode { ALPHAMODE_OPAQUE, ALPHAMODE_MASK, ALPHAMODE_BLEND };
		AlphaMode alphaMode = ALPHAMODE_OPAQUE;
		float alphaCutoff = 1.0f;
		float metallicFactor = 1.0f;
		float roughnessFactor = 1.0f;
		glm::vec4 baseColorFactor = glm::vec4(1.0f);

		std::shared_ptr<Texture> baseColorTexture ;
		std::shared_ptr<Texture> metallicRoughnessTexture ;
		std::shared_ptr<Texture> normalTexture ;
		std::shared_ptr<Texture> occlusionTexture ;
		std::shared_ptr<Texture> emissiveTexture ;

		//std::shared_ptr<Texture> specularGlossinessTexture;
		//std::shared_ptr<Texture> diffuseTexture;

		//void createDescriptorSet(GLTFSys& sys);
		std::unique_ptr<vk::DescriptorSet> descSet;
		std::unique_ptr<vk::DescriptorSetLayout> descSetLayout;

		Material() {};
		
	};

	/*
		glTF primitive
	*/
	struct Primitive {
		uint32_t firstIndex;
		uint32_t indexCount;
		uint32_t firstVertex;
		uint32_t vertexCount;
		//Material& material;
		uint32_t materialIndex;

		struct Dimensions {
			glm::vec3 min = glm::vec3(FLT_MAX);
			glm::vec3 max = glm::vec3(-FLT_MAX);
			glm::vec3 size;
			glm::vec3 center;
			float radius;
		} dimensions;

		void setDimensions(glm::vec3 min, glm::vec3 max)
		{
			dimensions.min = min;
			dimensions.max = max;
			dimensions.size = max - min;
			dimensions.center = (min + max) / 2.0f;
			dimensions.radius = glm::distance(min, max) / 2.0f;
		};
		Primitive(uint32_t firstIndex, uint32_t indexCount, uint32_t materialIndex) : firstIndex(firstIndex), indexCount(indexCount), materialIndex(materialIndex) {};
	};

	struct gltfMesh 
	{
		std::vector<Primitive*> primitives;
		std::string name;

		//std::unique_ptr<vk::Buffer> uniformBuffer;

		gltfMesh(glm::mat4) {};
		~gltfMesh();
	};

	struct Node 
	{
		Node* parent;
		uint32_t index;
		std::vector<Node*> children;
		
		std::string name;
		gltfMesh* mesh;

		// push constant: node's local position
		glm::mat4 matrix;
		glm::vec3 translation{};
		glm::vec3 scale{ 1.0f };
		glm::quat rotation{};
		glm::mat4 localMatrix();
		glm::mat4 getMatrix();
		void update();
		~Node();
	};
	class GLTFModel
	{
	public:
		GLTFModel(GLTFLoader& loader,const std::string& file, float scale = 1.0f);
		~GLTFModel() {};
		void Clean();

		struct Dimensions {
			glm::vec3 min = glm::vec3(FLT_MAX);
			glm::vec3 max = glm::vec3(-FLT_MAX);
			glm::vec3 size;
			glm::vec3 center;
			float radius;
		} dimensions;

		bool metallicRoughnessWorkflow = true;
		bool buffersBound = false;
		std::string path;
		

		const std::vector<Material*>& GetMatirials() const { return m_materials; }
		const std::vector<Node*>& GetNodes() const { return m_nodes; }

		const vk::Buffer& GetVertexBuffer() const { return *m_vertexBuffer; }
		const vk::Buffer& GetIndexBuffer() const { return *m_indexBuffer; }

	private:
		void loadFromFile(const std::string& filename, float scale);
		
		void loadImages(tinygltf::Model& input);
		void loadTextures(tinygltf::Model& input);
		std::shared_ptr<Texture> getImage(uint32_t index);

		void loadMaterials(tinygltf::Model& gltfModel);
		
		void loadNode(Node* parent, const tinygltf::Node& node, uint32_t nodeIndex, const tinygltf::Model& model, std::vector<uint32_t>& indexBuffer, std::vector<Vertex>& vertexBuffer, float globalscale);
	
	private:
		// image : source + sampler = Alalba::Texture
		std::vector<std::shared_ptr<Texture>> m_images;
		std::vector<uint32_t> m_textures;
		std::vector<Material*> m_materials;
		std::vector<Node*> m_nodes;

		std::unique_ptr<vk::Buffer> m_vertexBuffer;
		std::unique_ptr<vk::Buffer> m_indexBuffer;
		uint32_t m_vertexCount;
		uint32_t m_indexCount;

		GLTFLoader& m_loader;

	};
}
