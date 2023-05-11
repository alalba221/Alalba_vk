#version 450


layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 1) uniform sampler2D texSampler;


void main() 
{
    outColor = vec4(texture(texSampler,fragTexCoord).rgb,1.0);
    //outColor = vec4(Normal,1.0);
}