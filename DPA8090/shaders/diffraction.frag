#version 450


layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

// layout(set = 1, binding = 0) uniform sampler2D texSampler;
layout(set = 0, binding = 0) uniform  CameraBuffer{
	
	mat4 model;
	mat4 view;
	mat4 proj;
	vec3 pos;
} ubo;

struct DirLight 
{
  vec3 direction;
  vec3 color;
};  

vec3 light_dirrection_world  = vec3 (1.0, 1.0, 1.0);

DirLight directionLight = DirLight(light_dirrection_world,vec3 (1.0, 1.0, 1.0));



float saturate (float x)
{
    return min(1.0, max(0.0,x));
}
vec3 saturate (vec3 x)
{
    return min(vec3(1.,1.,1.), max(vec3(0.,0.,0.),x));
}

// --- Spectral Zucconi --------------------------------------------
// By Alan Zucconi
// Based on GPU Gems: https://developer.nvidia.com/sites/all/modules/custom/gpugems/books/GPUGems/gpugems_ch08.html
// But with values optimised to match as close as possible the visible spectrum
// Fits this: https://commons.wikimedia.org/wiki/File:Linear_visible_spectrum.svg
// With weighter MSE (RGB weights: 0.3, 0.59, 0.11)
vec3 bump3y (vec3 x, vec3 yoffset)
{
	vec3 y = vec3(1.,1.,1.) - x * x;
	y = saturate(y-yoffset);
	return y;
}
vec3 spectral_zucconi (float w)
{
   if(w > 700.0 || w < 400.0){
        return vec3(0);
    }
	float x = saturate((w - 400.0)/ 300.0);

	const vec3 c1 = vec3(3.54585104, 2.93225262, 2.41593945);
	const vec3 x1 = vec3(0.69549072, 0.49228336, 0.27699880);
	const vec3 y1 = vec3(0.02312639, 0.15225084, 0.52607955);

	const vec3 c2 = vec3(3.90307140, 3.21182957, 3.96587128);
	const vec3 x2 = vec3(0.11748627, 0.86755042, 0.66077860);
	const vec3 y2 = vec3(0.84897130, 0.88445281, 0.73949448);

	vec3 col = bump3y(c1 * (x - x1), y1) + bump3y(c2 * (x - x2), y2);
    return col;

}


vec3 getIridescentColour(vec3 rayDir, vec3 normal, vec3 lightDir, vec3 gratingDir, float d)
{
    vec3 colour = vec3(0);
    
    if(dot(normal, lightDir) < 0.0 || dot(normal, rayDir) < 0.0){
        return colour;
    }

    float sinThetaL = dot(gratingDir, lightDir);
    float sinThetaV = dot(gratingDir, rayDir);

    float u = abs(sinThetaL - sinThetaV);
    if(u == 0.0){
        return vec3(0);
    }

    for(int n = 1; n <= 16; n++){
        float wavelength = u * d / float(n);
        colour += spectral_zucconi(wavelength);
    }
    return saturate(colour);
}

void main() 
{
   
    vec3 world_position = Position;

    vec3 slit_direction = cross(world_position, Normal);


    vec3 light_second  = vec3 (0.0, -1.0, 0.0);

    outColor = vec4(getIridescentColour(normalize(ubo.pos), Normal,light_dirrection_world,slit_direction,700),1.0);
    outColor += vec4(getIridescentColour(normalize(ubo.pos), Normal,light_second,slit_direction,700),1.0);
}