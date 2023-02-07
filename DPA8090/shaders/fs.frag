#version 410

// Demonstrate Blinn-Phong shading model here.

// Inputs from application.
// Generally, "in" like the eye and normal vectors for things that change frequently,
// and "uniform" for things that change less often (think scene versus vertices).  

in vec3 position_eye, normal_eye;
uniform mat4 view_mat;

// This light setup would usually be passed in from the application.

vec3 light_position_world  = vec3 (10.0, 25.0, 10.0);
vec3 Ls = vec3 (1.0, 1.0, 1.0);    // neutral, full specular color of light
vec3 Ld = vec3 (0.8, 0.8, 0.8);    // neutral, lessened diffuse light color of light
vec3 La = vec3 (0.12, 0.12, 0.12); // ambient color of light - just a bit more than dk gray bg
  
// Surface reflectance properties for Phong or Blinn-Phong shading models below.

vec3 Ks = vec3 (1.0, 1.0, 1.0);    // fully reflect specular light
vec3 Kd = vec3 (0.32, 0.18, 0.5);  // purple diffuse surface reflectance
vec3 Ka = vec3 (1.0, 1.0, 1.0);    // fully reflect ambient light

float specular_exponent = 400.0;   // specular 'power' -- controls "roll-off"

// Shader programs can also designate outputs -- in this case the final pixel color to draw.
out vec4 fragment_color;           // color of surface to draw in this case

void main () {

  fragment_color = vec4 (Kd, 1.0);

}
