#include "pch.h"
#include "Vertex.h"
namespace Alalba
{
	std::array<VkVertexInputBindingDescription, 1> Vertex::binding_descriptions = 
	{ 
		VkVertexInputBindingDescription{}
	};
	
	std::array<VkVertexInputAttributeDescription, 5> Vertex::attribute_descriptions =
	{ 
		VkVertexInputAttributeDescription{}, VkVertexInputAttributeDescription{}, 
		VkVertexInputAttributeDescription{}, VkVertexInputAttributeDescription{}, 
		VkVertexInputAttributeDescription{} 
	};

}