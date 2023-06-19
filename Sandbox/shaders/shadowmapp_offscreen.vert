#version 450

layout (location = 0) in vec3 inPos;
layout( push_constant ) uniform constants
{
	mat4 model;
} PushConstants;
layout (binding = 0) uniform UBO 
{
	mat4 depthView;
	mat4 depthProject;
} ubo;

out gl_PerVertex 
{
    vec4 gl_Position;   
};

 
void main()
{
	gl_Position =  ubo.depthProject * ubo.depthView * PushConstants.model*vec4(inPos, 1.0);
}