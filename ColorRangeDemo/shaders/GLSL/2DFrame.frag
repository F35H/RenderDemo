#version 450

layout(binding = 0) uniform sampler2D imageSampler;

layout (location = 0) out vec4 outFragColor;

void main() 
{
	outFragColor = vec4(texture(imageSampler, vec2(1.0f,1.0f)).rgb, 1.0f);
}