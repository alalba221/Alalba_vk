#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;


layout(location = 0) out vec3 Position;
layout(location = 1) out vec3 Normal;
layout(location = 2) out vec2 fragTexCoord;


layout(set = 0, binding = 0) uniform  CameraBuffer{
	
	mat4 model;
	mat4 view;
	mat4 proj;
	vec3 pos;
} ubo;

void main() {
	
	gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
	// gl_Position = vec4(inPosition, 1.0);
	Position =  mat3(ubo.model) * inPosition;
	Normal = mat3(ubo.model) * inNormal;
	fragTexCoord = inTexCoord;
}