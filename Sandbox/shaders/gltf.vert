#version 450
#extension GL_EXT_debug_printf : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;


layout(location = 0) out vec3 Position;
layout(location = 1) out vec3 Normal;
layout(location = 2) out vec2 fragTexCoord;
layout (location = 3) out vec4 outShadowCoord;

layout( push_constant ) uniform constants
{
	mat4 model;
} PushConstants;

layout(set = 0, binding = 0) uniform  CameraLightBuffer{
	
	mat4 view;
	mat4 proj;

	mat4 lightview;
	mat4 lightproj;

	vec4 pos;

	vec4 lightPos;
	vec4 lightColor;
} ubo;

const mat4 biasMat = mat4( 
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0 );

void main() {
	
	gl_Position = ubo.proj * ubo.view * PushConstants.model * vec4(inPosition, 1.0);
	// gl_Position = vec4(inPosition, 1.0);
	Position =  mat3(PushConstants.model) * inPosition;
	Normal = mat3(PushConstants.model) * inNormal;
	fragTexCoord = inTexCoord;

	outShadowCoord = ( biasMat * ubo.lightproj * ubo.lightview * PushConstants.model ) * vec4(inPosition, 1.0);	
}