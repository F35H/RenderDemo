#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo; //UBO

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNorm;

layout(location = 0) flat out vec3 flatFragColor;
layout(location = 1) smooth out vec3 smoothFragColor;
layout(location = 2) out vec3 fragNorm;
layout(location = 3) out vec3 fragVert;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    
    //vec4 interNorm = transpose(inverse(ubo.model)) * vec4(inNorm, 1.0f);
    
    fragNorm = vec3(ubo.model * vec4(inNorm, 1.0f));
    fragVert = vec3(ubo.model * vec4(inPosition, 1.0f));
    flatFragColor = inColor;
    smoothFragColor = inColor;
}