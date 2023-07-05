#version 450


layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 fragTexCoord;
layout (location = 3) in vec4 inShadowCoord;

layout(location = 0) out vec4 outColor; // Output attachment 0
// layout(location = 1) out vec4 outDepth; // Output attachment 1


layout(set = 1, binding = 0) uniform sampler2D texSampler;
layout(set = 1, binding = 1) uniform sampler2D normalSampler;

layout(set = 0, binding = 0) uniform  CameraLightBuffer{
	
	mat4 view;
	mat4 proj;

	mat4 lightview;
	mat4 lightproj;

	vec4 pos;

	vec4 lightPos;
	vec4 lightColor;
} ubo;
layout (set = 0, binding = 1) uniform sampler2D shadowMap;

#define ambient 0.1
float textureProj(vec4 shadowCoord, vec2 off)
{
	float shadow = 1.0;
	if ( shadowCoord.z > -1.0 && shadowCoord.z < 1.0 ) 
	{
		float dist = texture( shadowMap, shadowCoord.st + off ).r;
		if ( shadowCoord.w > 0.0 && dist < shadowCoord.z ) 
		{
			shadow = ambient;
		}
	}
	return shadow;
}

// Light sources also have different intensities for their ambient, diffuse and specular components respectively
vec4 Ls = ubo.lightColor;    // neutral, full specular color of light
vec4 Ld = vec4 (0.8, 0.8, 0.8, 0.0);    // neutral, lessened diffuse light color of light
vec4 La = vec4 (0.12, 0.12, 0.12, 0.0); // ambient color of light - just a bit more than dk gray bg
float specular_exponent = 400.0;   // specular 'power' -- controls "roll-off"

// Material 
vec4 Ks = vec4 (1.0, 1.0, 1.0, 0);    // fully reflect specular light
vec4 Kd = vec4 (1.0, 1.0, 1.0, 0);  // purple diffuse surface reflectance
vec4 Ka = vec4 (1.0, 1.0, 1.0, 0);    // fully reflect ambient light


vec4 CalcPointLight(vec4 lightPos, vec4 lightColor, vec3 norm, vec3 pos, vec3 viewDir)
{
  vec3 lightDir = normalize(vec3(lightPos)-pos);

  // ambiant
  vec4 ambiant = Ka * La;

  // diffuse
  float diff = max(dot(norm,lightDir), 0.0);
  vec4 diffuse = Kd* Ld * diff;

  // specular
  vec3 reflectDir = reflect(-lightDir, norm);  
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), specular_exponent);
  vec4 specular = Ks * lightColor * spec;  

  return ambiant + diffuse + specular;
}


void main() 
{   
   // outDepth = 0;
     // outColor = vec4(texture(texSampler,fragTexCoord).rgb,1.0);
	outColor = vec4(1.0,0.0,0.0,1.0);

	vec3 normal = normalize(Normal);
	vec3 viewDir = normalize(vec3(ubo.pos) - Position);
 
	vec4 result = CalcPointLight(ubo.lightPos, Ls, normal,Position,viewDir);
  
	float shadow = textureProj(inShadowCoord / inShadowCoord.w, vec2(0.0));
//	result += CalcDirLight(dirG, normal, viewDir);
//	result += CalcDirLight(dirR, normal, viewDir);
	//outColor = vec4(texture(texSampler,fragTexCoord).rgb,1.0)*vec4 (result);
	outColor = vec4 (result.r*shadow,result.g*shadow,result.b*shadow,1.0);
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