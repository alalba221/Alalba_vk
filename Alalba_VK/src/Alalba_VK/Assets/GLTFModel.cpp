#include "pch.h"
#include "GLTFModel.h"
#include "GLTFLoader.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Alalba_VK/Core/Application.h"

#include "Alalba_VK/Vulkan/PipelineLayout.h"
#include "Alalba_VK/Vulkan/GraphicsPipeline.h"
namespace Alalba
{
	GLTFModel::GLTFModel(GLTFLoader& loader, const std::string& filename, float scale )
		:m_loader(loader)
	{
		loadFromFile(filename, scale);
	}

	void GLTFModel::DrawModel(const glm::mat4& basetransform, const vk::GraphicsPipeline& pipeline, const vk::CommandBuffers& cmdbuffers, const int currentCmdBuffer, uint32_t bindImageSet) const
	{
		// 1. Get vertex and index buffer
		VkBuffer vertexBuffers[] = { m_vertexBuffer->Handle() };
		VkBuffer IndexBuffers = m_indexBuffer->Handle();

		// 2. Bind Vertex and index buffer
		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(cmdbuffers[currentCmdBuffer], 0, 1, vertexBuffers, &offset);
		vkCmdBindIndexBuffer(cmdbuffers[currentCmdBuffer], IndexBuffers, offset, VK_INDEX_TYPE_UINT32);

		for (auto node : m_nodes)
		{
			DrawNode(basetransform, pipeline, node, cmdbuffers, currentCmdBuffer, bindImageSet);
		}
	}

	void GLTFModel::DrawNode(const glm::mat4& basetransform, const vk::GraphicsPipeline& pipeline,const Node* node, const vk::CommandBuffers& cmdBuffers, const int currentCmdBuffer, uint32_t bindImageSet) const
	{
		if (node->mesh)
		{
			glm::mat4 nodeMatrix = node->matrix;
			Node* currentParent = node->parent;
			while (currentParent)
			{
				nodeMatrix = currentParent->matrix * nodeMatrix;
				currentParent = currentParent->parent;
			}

			// Pass the final matrix to the vertex shader using push constants
			nodeMatrix = basetransform * nodeMatrix;
			vkCmdPushConstants(cmdBuffers[currentCmdBuffer], pipeline.GetPipelineLayout().Handle(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &nodeMatrix);
			for (Primitive* primitive : node->mesh->primitives) {
				// bool skip = false;
				uint32_t materialIndex = primitive->materialIndex;
				//if (renderFlags & RenderFlags::RenderOpaqueNodes) {
				//	skip = (material.alphaMode != Material::ALPHAMODE_OPAQUE);
				//}
				//if (renderFlags & RenderFlags::RenderAlphaMaskedNodes) {
				//	skip = (material.alphaMode != Material::ALPHAMODE_MASK);
				//}
				//if (renderFlags & RenderFlags::RenderAlphaBlendedNodes) {
				//	skip = (material.alphaMode != Material::ALPHAMODE_BLEND);
				//}
				//if (!skip) {
				//	if (renderFlags & RenderFlags::BindImages) {
				//		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, bindImageSet, 1, &material.descriptorSet, 0, nullptr);
				//	}
				//	vkCmdDrawIndexed(commandBuffer, primitive->indexCount, 1, primitive->firstIndex, 0, 0);
				//}

				//
				uint32_t materialID = primitive->materialIndex;
				Material* material = m_materials.at(materialID);
				VkDescriptorSet materialSet = material->descSet->Handle();
				vkCmdBindDescriptorSets(cmdBuffers[currentCmdBuffer], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.GetPipelineLayout().Handle(), bindImageSet, 1, &materialSet, 0, nullptr);
				vkCmdDrawIndexed(cmdBuffers[currentCmdBuffer], primitive->indexCount, 1, primitive->firstIndex, 0, 0);
			}
		}
		for (auto& child : node->children) {
			DrawNode(basetransform, pipeline, child, cmdBuffers, currentCmdBuffer);
		}
	}

	void GLTFModel::loadFromFile(const std::string& filename, float scale)
	{

		tinygltf::Model gltfModel;
		tinygltf::TinyGLTF gltfLoader;
		//gltfContext.SetImageLoader
		size_t pos = filename.find_last_of('/');
		path = filename.substr(0, pos);
		
		ALALBA_TRACE("loading model {0}", filename);
		std::string error, warning;
		bool fileLoaded = gltfLoader.LoadASCIIFromFile(&gltfModel, &error, &warning, filename);

		std::vector<uint32_t> indexBuffer;
		std::vector<Vertex> vertexBuffer;

		if (fileLoaded)
		{
			/// 0. load texture from gltfModel into m_textures array
			loadImages(gltfModel);
			/// 1. assign texture to materials
			loadMaterials(gltfModel);
			/// 2. load node into m_nodes:(decompress each primitive for mesh from file into buffers)
			// TODO: for now only one scene for the model
			const tinygltf::Scene& scene = gltfModel.scenes[gltfModel.defaultScene > -1 ? gltfModel.defaultScene : 0];
			for (size_t i = 0; i < scene.nodes.size(); i++) 
			{
				const tinygltf::Node node = gltfModel.nodes[scene.nodes[i]];
				loadNode(nullptr, node, scene.nodes[i], gltfModel, indexBuffer, vertexBuffer, scale);
			}
		}
		else {
			ALALBA_ASSERT("COUNLDNT LOAD GLTF");
		}

		/// 3. vertex buffer and index buffer 
		size_t vertexBufferSize = vertexBuffer.size() * sizeof(Vertex);
		size_t indexBufferSize = indexBuffer.size() * sizeof(uint32_t);
		m_indexCount = static_cast<uint32_t>(indexBuffer.size());
		m_vertexCount = static_cast<uint32_t>(vertexBuffer.size());
	
		m_vertexBuffer = vk::Buffer::Builder(Application::Get().GetDevice(), m_loader.Allocator())
			.SetTag("Vertex Buffer")
			.SetSize(vertexBufferSize)
			.SetUsage(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)
			.SetVmaUsage(VMA_MEMORY_USAGE_GPU_ONLY)
			.Build();
		/*
			https://vulkan-tutorial.com/Vertex_buffers/Staging_buffer#page_Transfer-queue
			Use graphic queue, since both compute and graphics q support transfer
			m_copyCmdBuffer->Flush(0, Application::Get().GetDevice().GetTransferQ());
		*/
		m_vertexBuffer->CopyDataFrom(
			vertexBuffer.data(), vertexBufferSize,
			Application::Get().GetDevice().GetGraphicsQ(), m_loader.CmdPool()
		);
		m_indexBuffer = vk::Buffer::Builder(Application::Get().GetDevice(), m_loader.Allocator())
			.SetTag("Index Buffer")
			.SetSize(indexBufferSize)
			.SetUsage(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
			.SetVmaUsage(VMA_MEMORY_USAGE_GPU_ONLY)
			.Build();

		m_indexBuffer->CopyDataFrom(
			indexBuffer.data(), indexBufferSize,
			Application::Get().GetDevice().GetGraphicsQ(), m_loader.CmdPool()
		);
		/// 4. Descriptors for per-node uniform buffers
		
		/// 5. Descriptors for per-material images
		for (auto& material : m_materials)
		{
			if (material->baseColorTexture != nullptr) // not the last empty material
			{
				material->descSetLayout = vk::DescriptorSetLayout::Builder(Application::Get().GetDevice())
					// 0 : is bingding index of the binding slot in the set
					.AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
					.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
					.AddBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
					.AddBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
					.SetTag("material descriptor Set Layout")
					.Build();

				material->descSet = vk::DescriptorSet::Allocator(Application::Get().GetDevice(), m_loader.DescPool())
					.SetTag("Material Descritor Set ")
					.SetDescSetLayout(*material->descSetLayout)
					.Allocate();

				material->descSet																																																																				// VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
					->BindDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, material->baseColorTexture->GetSampler(), material->baseColorTexture->GetImageView(), material->baseColorTexture->GetImage().Layout())
					.BindDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, material->normalTexture->GetSampler(), material->normalTexture->GetImageView(), material->normalTexture->GetImage().Layout())
					.UpdateDescriptors()
					;
			}
		}
	}

	void GLTFModel::loadNode(Node* parent, const tinygltf::Node& node, uint32_t nodeIndex, const tinygltf::Model& model, 
		std::vector<uint32_t>& indexBuffer, std::vector<Vertex>& vertexBuffer, float globalscale)
	{
		Node* newNode = new Node{};
		newNode->index = nodeIndex;
		newNode->parent = parent;
		newNode->name = node.name;
		newNode->matrix = glm::mat4(1.0f);

		// Generate local node matrix
		glm::vec3 translation = glm::vec3(0.0f);
		if (node.translation.size() == 3) {
			translation = glm::make_vec3(node.translation.data());
			newNode->translation = translation;
		}
		glm::mat4 rotation = glm::mat4(1.0f);
		if (node.rotation.size() == 4) {
			//glm::quat q = glm::make_quat(node.rotation.data());
			glm::quat q = glm::quat(node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]);
			newNode->rotation = glm::mat4(q);
		}
		glm::vec3 scale = glm::vec3(1.0f);
		if (node.scale.size() == 3) {
			scale = glm::make_vec3(node.scale.data());
			newNode->scale = scale;
		}
		if (node.matrix.size() == 16) {
			newNode->matrix = glm::make_mat4x4(node.matrix.data());
			if (globalscale != 1.0f) {
				newNode->matrix = glm::scale(newNode->matrix, glm::vec3(globalscale));
			}
		};

		newNode->matrix = glm::translate(glm::mat4(1.0f), newNode->translation)* glm::mat4(newNode->rotation)* glm::scale(glm::mat4(1.0f), newNode->scale)* newNode->matrix;

		// Node with children
		if (node.children.size() > 0) 
		{
			for (auto i = 0; i < node.children.size(); i++) 
			{
				loadNode(newNode, model.nodes[node.children[i]], node.children[i], model, indexBuffer, vertexBuffer, globalscale);
			}
		}

		// Node contains mesh data
		if (node.mesh > -1) 
		{
			const tinygltf::Mesh mesh = model.meshes[node.mesh];
			gltfMesh* newMesh = new gltfMesh(newNode->matrix);
			newMesh->name = mesh.name;
			// The meshes may contain multiple mesh primitives
			for (size_t j = 0; j < mesh.primitives.size(); j++) 
			{
				const tinygltf::Primitive& primitive = mesh.primitives[j];
				if (primitive.indices < 0) {
					continue;
				}
				uint32_t indexStart = static_cast<uint32_t>(indexBuffer.size());
				uint32_t vertexStart = static_cast<uint32_t>(vertexBuffer.size());
				uint32_t indexCount = 0;
				uint32_t vertexCount = 0;
				glm::vec3 posMin{};
				glm::vec3 posMax{};
				bool hasSkin = false;
				// Vertices
				{
					const float* bufferPos = nullptr;
					const float* bufferNormals = nullptr;
					const float* bufferTexCoords = nullptr;
					const float* bufferColors = nullptr;
					const float* bufferTangents = nullptr;
					uint32_t numColorComponents;
					const uint16_t* bufferJoints = nullptr;
					const float* bufferWeights = nullptr;

					// Position attribute is required
					assert(primitive.attributes.find("POSITION") != primitive.attributes.end());

					const tinygltf::Accessor& posAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
					const tinygltf::BufferView& posView = model.bufferViews[posAccessor.bufferView];
					bufferPos = reinterpret_cast<const float*>(&(model.buffers[posView.buffer].data[posAccessor.byteOffset + posView.byteOffset]));
					posMin = glm::vec3(posAccessor.minValues[0], posAccessor.minValues[1], posAccessor.minValues[2]);
					posMax = glm::vec3(posAccessor.maxValues[0], posAccessor.maxValues[1], posAccessor.maxValues[2]);

					if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
						const tinygltf::Accessor& normAccessor = model.accessors[primitive.attributes.find("NORMAL")->second];
						const tinygltf::BufferView& normView = model.bufferViews[normAccessor.bufferView];
						bufferNormals = reinterpret_cast<const float*>(&(model.buffers[normView.buffer].data[normAccessor.byteOffset + normView.byteOffset]));
					}

					if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
						const tinygltf::Accessor& uvAccessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
						const tinygltf::BufferView& uvView = model.bufferViews[uvAccessor.bufferView];
						bufferTexCoords = reinterpret_cast<const float*>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
					}

					if (primitive.attributes.find("COLOR_0") != primitive.attributes.end())
					{
						const tinygltf::Accessor& colorAccessor = model.accessors[primitive.attributes.find("COLOR_0")->second];
						const tinygltf::BufferView& colorView = model.bufferViews[colorAccessor.bufferView];
						// Color buffer are either of type vec3 or vec4
						numColorComponents = colorAccessor.type == TINYGLTF_PARAMETER_TYPE_FLOAT_VEC3 ? 3 : 4;
						bufferColors = reinterpret_cast<const float*>(&(model.buffers[colorView.buffer].data[colorAccessor.byteOffset + colorView.byteOffset]));
					}

					if (primitive.attributes.find("TANGENT") != primitive.attributes.end())
					{
						const tinygltf::Accessor& tangentAccessor = model.accessors[primitive.attributes.find("TANGENT")->second];
						const tinygltf::BufferView& tangentView = model.bufferViews[tangentAccessor.bufferView];
						bufferTangents = reinterpret_cast<const float*>(&(model.buffers[tangentView.buffer].data[tangentAccessor.byteOffset + tangentView.byteOffset]));
					}

					// Skinning
					// Joints
					if (primitive.attributes.find("JOINTS_0") != primitive.attributes.end()) {
						const tinygltf::Accessor& jointAccessor = model.accessors[primitive.attributes.find("JOINTS_0")->second];
						const tinygltf::BufferView& jointView = model.bufferViews[jointAccessor.bufferView];
						bufferJoints = reinterpret_cast<const uint16_t*>(&(model.buffers[jointView.buffer].data[jointAccessor.byteOffset + jointView.byteOffset]));
					}

					if (primitive.attributes.find("WEIGHTS_0") != primitive.attributes.end()) {
						const tinygltf::Accessor& uvAccessor = model.accessors[primitive.attributes.find("WEIGHTS_0")->second];
						const tinygltf::BufferView& uvView = model.bufferViews[uvAccessor.bufferView];
						bufferWeights = reinterpret_cast<const float*>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
					}

					hasSkin = (bufferJoints && bufferWeights);

					vertexCount = static_cast<uint32_t>(posAccessor.count);

					for (size_t v = 0; v < posAccessor.count; v++) {
						Vertex vert{};
						vert.position = glm::vec4(glm::make_vec3(&bufferPos[v * 3]), 1.0f);
						vert.normal = glm::normalize(glm::vec3(bufferNormals ? glm::make_vec3(&bufferNormals[v * 3]) : glm::vec3(0.0f)));
						vert.uv = bufferTexCoords ? glm::make_vec2(&bufferTexCoords[v * 2]) : glm::vec3(0.0f);
						if (bufferColors) {
							switch (numColorComponents) {
							case 3:
								vert.color = glm::vec4(glm::make_vec3(&bufferColors[v * 3]), 1.0f);
							case 4:
								vert.color = glm::make_vec4(&bufferColors[v * 4]);
							}
						}
						else {
							//vert.color = glm::vec4(1.0f);
						}
						vert.tangent = bufferTangents ? glm::vec4(glm::make_vec4(&bufferTangents[v * 4])) : glm::vec4(0.0f);
						//vert.joint0 = hasSkin ? glm::vec4(glm::make_vec4(&bufferJoints[v * 4])) : glm::vec4(0.0f);
						//vert.weight0 = hasSkin ? glm::make_vec4(&bufferWeights[v * 4]) : glm::vec4(0.0f);
						vertexBuffer.push_back(vert);
					}
				}
				// Indices
				{
					const tinygltf::Accessor& accessor = model.accessors[primitive.indices];
					const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
					const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

					indexCount = static_cast<uint32_t>(accessor.count);

					switch (accessor.componentType) {
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
						uint32_t* buf = new uint32_t[accessor.count];
						memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint32_t));
						for (size_t index = 0; index < accessor.count; index++) {
							indexBuffer.push_back(buf[index] + vertexStart);
						}
						delete[] buf;
						break;
					}
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
						uint16_t* buf = new uint16_t[accessor.count];
						memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint16_t));
						for (size_t index = 0; index < accessor.count; index++) {
							indexBuffer.push_back(buf[index] + vertexStart);
						}
						delete[] buf;
						break;
					}
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
						uint8_t* buf = new uint8_t[accessor.count];
						memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint8_t));
						for (size_t index = 0; index < accessor.count; index++) {
							indexBuffer.push_back(buf[index] + vertexStart);
						}
						delete[] buf;
						break;
					}
					default:
						std::cerr << "Index component type " << accessor.componentType << " not supported!" << std::endl;
						return;
					}
				}

				Primitive* newPrimitive = new Primitive(indexStart, indexCount, primitive.material > -1 ? primitive.material : m_materials.size()-1);
				newPrimitive->firstVertex = vertexStart;
				newPrimitive->vertexCount = vertexCount;
				newPrimitive->setDimensions(posMin, posMax);
				newMesh->primitives.push_back(newPrimitive);
			}
			newNode->mesh = newMesh;
		}
		if (parent) {
			parent->children.push_back(newNode);
		}
		else {
			m_nodes.push_back(newNode);
		}
		//linearNodes.push_back(newNode);


	}

	void GLTFModel::loadMaterials(tinygltf::Model& gltfModel)
	{
		for (tinygltf::Material& mat : gltfModel.materials) 
		{
			Material* material = new Material();
			if (mat.values.find("baseColorTexture") != mat.values.end()) {
				material->baseColorTexture = getImage(gltfModel.textures[mat.values["baseColorTexture"].TextureIndex()].source);
			}else{
				material->baseColorTexture = m_emptyTexture.get();
			}

			// Metallic roughness workflow
			if (mat.values.find("metallicRoughnessTexture") != mat.values.end()) {
				material->metallicRoughnessTexture = getImage(gltfModel.textures[mat.values["metallicRoughnessTexture"].TextureIndex()].source);
			}else {
				material->metallicRoughnessTexture = m_emptyTexture.get();
			}
			if (mat.values.find("roughnessFactor") != mat.values.end()) {
				material->roughnessFactor = static_cast<float>(mat.values["roughnessFactor"].Factor());
			}
			if (mat.values.find("metallicFactor") != mat.values.end()) {
				material->metallicFactor = static_cast<float>(mat.values["metallicFactor"].Factor());
			}
			if (mat.values.find("baseColorFactor") != mat.values.end()) {
				material->baseColorFactor = glm::make_vec4(mat.values["baseColorFactor"].ColorFactor().data());
			}
			if (mat.additionalValues.find("normalTexture") != mat.additionalValues.end()) {
				material->normalTexture = getImage(gltfModel.textures[mat.additionalValues["normalTexture"].TextureIndex()].source);
			}else {
				material->normalTexture = m_emptyTexture.get();
			}
			if (mat.additionalValues.find("emissiveTexture") != mat.additionalValues.end()) {
				material->emissiveTexture = getImage(gltfModel.textures[mat.additionalValues["emissiveTexture"].TextureIndex()].source);
			}else {
				material->emissiveTexture = m_emptyTexture.get();
			}
			if (mat.additionalValues.find("occlusionTexture") != mat.additionalValues.end()) {
				material->occlusionTexture = getImage(gltfModel.textures[mat.additionalValues["occlusionTexture"].TextureIndex()].source);
			}else {
				material->occlusionTexture = m_emptyTexture.get();
			}
			if (mat.additionalValues.find("alphaMode") != mat.additionalValues.end()) {
				tinygltf::Parameter param = mat.additionalValues["alphaMode"];
				if (param.string_value == "BLEND") {
					material->alphaMode = Material::ALPHAMODE_BLEND;
				}
				if (param.string_value == "MASK") {
					material->alphaMode = Material::ALPHAMODE_MASK;
				}
			}
			if (mat.additionalValues.find("alphaCutoff") != mat.additionalValues.end()) {
				material->alphaCutoff = static_cast<float>(mat.additionalValues["alphaCutoff"].Factor());
			}

			m_materials.push_back(material);
		}
		// Push a default material at the end of the list for meshes with no material assigned
		// m_materials.push_back(new Material());
	}
	void GLTFModel::loadImages(tinygltf::Model& gltfModel) 
	{
		// Images can be stored inside the glTF (which is the case for the sample model), so instead of directly
	  // loading them from disk, we fetch them from the glTF loader and upload the buffers
		m_images.resize(gltfModel.images.size());
		for (size_t i = 0; i < gltfModel.images.size(); i++)
		{
			tinygltf::Image& glTFImage = gltfModel.images[i];
			// Get the image data from the glTF loader
			unsigned char* buffer = nullptr;
			VkDeviceSize   bufferSize = 0;
			bool           deleteBuffer = false;
			// We convert RGB-only images to RGBA, as most devices don't support RGB-formats in Vulkan
			if (glTFImage.component == 3)
			{
				bufferSize = glTFImage.width * glTFImage.height * 4;
				buffer = new unsigned char[bufferSize];
				unsigned char* rgba = buffer;
				unsigned char* rgb = &glTFImage.image[0];
				for (size_t i = 0; i < glTFImage.width * glTFImage.height; ++i)
				{
					memcpy(rgba, rgb, sizeof(unsigned char) * 3);
					rgba += 4;
					rgb += 3;
				}
				deleteBuffer = true;
			}
			else
			{
				buffer = &glTFImage.image[0];
				bufferSize = glTFImage.image.size();
			}
			// Load texture from image buffer
			m_images[i].reset(new Texture(buffer, bufferSize, VK_FORMAT_R8G8B8A8_UNORM, glTFImage.width, glTFImage.height,
				m_loader.Allocator(), m_loader.CmdPool()));
			
			if (deleteBuffer)
			{
				delete[] buffer;
			}
		}

		// Create an empty texture to be used for empty material images
		creatEmptyTexture();
	}

	void GLTFModel::creatEmptyTexture()
	{
		size_t bufferSize = 1 * 1 * 4;
		unsigned char* buffer = new unsigned char[bufferSize];
		memset(buffer, 0, bufferSize);
		m_emptyTexture.reset(new Texture(buffer, bufferSize, VK_FORMAT_R8G8B8A8_UNORM, 1, 1,
			m_loader.Allocator(), m_loader.CmdPool()));
	}


	Texture* GLTFModel::getImage(uint32_t index)
	{
		if (index < m_images.size()) {
			return m_images[index].get();
		}
		return nullptr;
	}

	void GLTFModel::Clean()
	{
		if (!m_images.empty())
		{
			for (auto& img : m_images)
				img->Clean();
		}
		if (!m_materials.empty())
		{
			for (auto mat : m_materials)
			{
				if (mat->baseColorTexture != nullptr) { // call clean except empty material
					mat->descSet->Clean();
					mat->descSetLayout->Clean();
				}
			}
		}
		m_vertexBuffer->Clean();
		m_indexBuffer->Clean();
	}

}
