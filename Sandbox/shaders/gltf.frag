#version 450


layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 1) out vec4 outColor; // Output attachment 0
// layout(location = 1) out vec4 outDepth; // Output attachment 1


layout(set = 1, binding = 0) uniform sampler2D texSampler;
layout(set = 1, binding = 1) uniform sampler2D normalSampler;

void main() 
{   
   // outDepth = 0;
    outColor = vec4(texture(texSampler,fragTexCoord).rgb,1.0);
}

//float near = 0.1; 
//float far  = 100.0; 
//  
//float LinearizeDepth(float depth) 
//{
//    float z = depth * 2.0 - 1.0; // back to NDC 
//    return (2.0 * near * far) / (far + near - z * (far - near));	
//}
//
//void main()
//{             
//    float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration
//    outColor = vec4(vec3(depth), 1.0);
//}