#version 450

layout (location = 0) in vec4 inColor;
layout (location = 1) in vec3 inPos;
layout (location = 2) in vec3 inNorm;

layout (binding = 0) uniform UBO 
{
	mat4 projection;
	mat4 view;
	mat4 model;
} ubo;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main() 
{
  //Extrude along Position 
	//gl_Position = ubo.projection * ubo.model * ubo.view * vec4(inPos.xyz + inPos.xyz * 0.1, 1.0f);
	
  //Extrude along position normal
  gl_Position = ubo.projection * ubo.model * ubo.view * vec4(inPos.xyz + normalize(inPos.xyz) * 0.1, 1.0f);

  // Vertical box blur.

  
  //Roberts Cross Edge Detection



  //Sobel Edge Detection
}