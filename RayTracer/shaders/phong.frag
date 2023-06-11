#version 450

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform  CameraBuffer{
	mat4 model;
	mat4 view;
	mat4 proj;
  vec3 pos;
} ubo;

layout(set = 1, binding = 0) uniform sampler2D texSampler;


vec3 light_position_world  = vec3 (10.0, 25.0, 10.0);
vec3 Ls = vec3 (1.0, 1.0, 1.0);    // neutral, full specular color of light
vec3 Ld = vec3 (0.8, 0.8, 0.8);    // neutral, lessened diffuse light color of light
vec3 La = vec3 (0.12, 0.12, 0.12); // ambient color of light - just a bit more than dk gray bg
  
// Surface reflectance properties for Phong or Blinn-Phong shading models below.

vec3 Ks = vec3 (1.0, 1.0, 1.0);    // fully reflect specular light
vec3 Kd = vec3 (0.32, 0.18, 0.5);  // purple diffuse surface reflectance
vec3 Ka = vec3 (1.0, 1.0, 1.0);    // fully reflect ambient light

float specular_exponent = 400.0;   // specular 'power' -- controls "roll-off"

void main() 
{
    
  // ambiant
  vec3 ambiant = Ka*La;

  // diffuse
  vec3 normal = normalize(Normal);
  vec3 lightDir = normalize(light_position_world-Position);
  float diff = max(dot(normal,lightDir), 0.0);
  vec3 diffuse = Kd*Ld*diff;

  // specular
  vec3 viewDir = normalize(ubo.pos - Position);
  vec3 reflectDir = reflect(-lightDir, normal);  
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), specular_exponent);
  vec3 specular = Ks*Ls*spec;  

  outColor = vec4 (ambiant + diffuse + specular , 1.0);
}