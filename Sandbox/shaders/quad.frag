#version 450

layout (binding = 1) uniform sampler2D samplerColor;

layout (location = 0) in vec2 inUV;

layout (location = 0) out vec4 outFragColor;

layout (binding = 0) uniform UBO 
{
//	mat4 projection;
//	mat4 view;
//	mat4 model;
//	mat4 lightSpace;
//	vec4 lightPos;
	float zNear;
	float zFar;
} ubo;

float zNear = 0.1f;
float zFar = 100.0;

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * zNear * zFar) / (zFar + zNear - z * (zFar - zNear));	
}

//float LinearizeDepth(float depth)
//{
//  float n = zNear;
//  float f = zFar;
//  float z = depth;
//  return (2.0 * n) / (f + n - z * (f - n));	
//}
//
void main() 
{
	float depth = LinearizeDepth(texture(samplerColor, inUV).r)/zFar;

	outFragColor = vec4(vec3(depth), 1.0);

}