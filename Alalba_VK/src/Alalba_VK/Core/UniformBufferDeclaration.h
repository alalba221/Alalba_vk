#pragma once
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Alalba
{
	using byte = unsigned char;

	enum class UniformType
	{
		None = 0,
		Float , Float2, Float3, Float4,
		Matrix3x3, Matrix4x4,
		Int32, Uint32
	};

	struct UniformDecl
	{
		UniformType Type;
		std::ptrdiff_t Offeset;
		std::string Name;
	};



	class UniformBufferDeclaration
	{
	public:
		UniformBufferDeclaration(uint32_t size) 
		{
			Buffer.resize(size);
		};

	private:
		std::vector<byte> Buffer;
		// byte Buffer[N];
		std::vector<UniformDecl> Uniforms;
		//UniformDecl Uniforms[U];

		std::ptrdiff_t Cursor = 0;
		//int Index = 0;
	public:
		const byte* GetBuffer() const { return Buffer.data(); }
		const UniformDecl* GetUniforms() const { return Uniforms.data(); }
		unsigned int GetUniformCount() const { return Uniforms.size(); }
		unsigned int GetBufferSize() const { return Buffer.size(); }
	

		template<typename T>
		void Push(const std::string& name, const T& data) {}

		/*template<>*/
		void Push(const std::string& name, const float& data)
		{
			glm::vec4 padded_data = glm::vec4(data, 0, 0, 0);
			//Uniforms.push_back( { UniformType::Float, Cursor, name } );
			memcpy(Buffer.data() + Cursor, &padded_data, sizeof(glm::vec4));
			Cursor += sizeof(glm::vec4);
		}

		void Push(const std::string& name, const glm::vec3& data)
		{
			glm::vec4 padded_data = glm::vec4(data, 0);
			//Uniforms.push_back({ UniformType::Float3, Cursor, name });
			memcpy(Buffer.data() + Cursor, &padded_data, sizeof(glm::vec4));
			Cursor += sizeof(glm::vec4);
		}

		//template<>
		void Push(const std::string& name, const glm::vec4& data)
		{
			//Uniforms.push_back({ UniformType::Float4, Cursor, name });
			memcpy(Buffer.data() + Cursor, &data, sizeof(glm::vec4));
			Cursor += sizeof(glm::vec4);
		}

		void Push(const std::string& name, const glm::mat4& data)
		{
			//Uniforms.push_back({ UniformType::Matrix4x4, Cursor, name });
			memcpy(Buffer.data() + Cursor, &data, sizeof(glm::mat4));
			Cursor += sizeof(glm::mat4);
		}
	};
}