#version 450
#extension GL_EXT_debug_printf : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;


layout(location = 0) out vec3 Position;
layout(location = 1) out vec3 Normal;
layout(location = 2) out vec2 fragTexCoord;

layout( push_constant ) uniform constants
{
	mat4 matrix;
} PushConstants;

layout(set = 0, binding = 0) uniform  CameraLightBuffer{
	
	mat4 view;
	mat4 proj;
	vec4 pos;

	vec4 lightPos;
	vec4 lightColor;
} ubo;



void main() {
	
	gl_Position = ubo.proj * ubo.view * PushConstants.matrix * vec4(inPosition, 1.0);
	// gl_Position = vec4(inPosition, 1.0);
	Position =  mat3(PushConstants.matrix) * inPosition;
	Normal = mat3(PushConstants.matrix) * inNormal;
	fragTexCoord = inTexCoord;
}