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


vec3 light_position_world  = vec3 (10.0, 10.0, 10.0);
vec3 Ls = vec3 (1.0, 1.0, 1.0);    // neutral, full specular color of light
vec3 Ld = vec3 (0.8, 0.8, 0.8);    // neutral, lessened diffuse light color of light
vec3 La = vec3 (0.12, 0.12, 0.12); // ambient color of light - just a bit more than dk gray bg
  
// Surface reflectance properties for Phong or Blinn-Phong shading models below.

vec3 Ks = vec3 (1.0, 1.0, 1.0);    // fully reflect specular light
vec3 Kd = vec3 (0.32, 0.18, 0.5);  // purple diffuse surface reflectance
vec3 Ka = vec3 (1.0, 1.0, 1.0);    // fully reflect ambient light

float specular_exponent = 400.0;   // specular 'power' -- controls "roll-off"

// material parameters
vec3 albedo = Kd;
float metallic = 0.7;
float roughness = 0.1;
float ao = 0.6;

const float PI = 3.14159265359;

struct DirLight 
{
  vec3 direction;
  vec3 color;
};  

struct SpotLight 
{
  vec3 position;
  vec3 color;
};

SpotLight spotLight = SpotLight(light_position_world,vec3 (1.0, 1.0, 1.0));


// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------

void main() 
{
  vec3 N = normalize(Normal);
  vec3 V = normalize(ubo.pos - Position);

  // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
  // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
  vec3 F0 = vec3(0.04); 
  F0 = mix(F0, albedo, metallic);
  
  // reflectance equation
  vec3 Lo = vec3(0.0);

  // calculate per-light radiance
  vec3 L = normalize(spotLight.position - Position);
  vec3 H = normalize(V + L);
  float distance = length(spotLight.position - Position);
  float attenuation = 1.0 / (distance * distance);
  vec3 radiance = spotLight.color * attenuation;
  
  // Cook-Torrance BRDF
  float NDF = DistributionGGX(N, H, roughness);   
  float G   = GeometrySmith(N, V, L, roughness);      
  vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
     
  vec3 numerator    = NDF * G * F; 
  float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
  vec3 specular = numerator / denominator;
  
  // kS is equal to Fresnel
  vec3 kS = F;
  // for energy conservation, the diffuse and specular light can't
  // be above 1.0 (unless the surface emits light); to preserve this
  // relationship the diffuse component (kD) should equal 1.0 - kS.
  vec3 kD = vec3(1.0) - kS;
  // multiply kD by the inverse metalness such that only non-metals 
  // have diffuse lighting, or a linear blend if partly metal (pure metals
  // have no diffuse light).
  kD *= 1.0 - metallic;	  
  
  // scale light by NdotL
  float NdotL = max(dot(N, L), 0.0);        
  
  // add to outgoing radiance Lo
  Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again

  
  // ambient lighting (note that the next IBL tutorial will replace 
  // this ambient lighting with environment lighting).
  vec3 ambient = vec3(0.03) * albedo * ao;
  
  vec3 color = ambient + Lo;
  
  // HDR tonemapping
  color = color / (color + vec3(1.0));
  // gamma correct
  color = pow(color, vec3(1.0/2.2)); 
  
  outColor = vec4(color, 1.0);
 
}