#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo; //UBO

layout(location = 0) in vec4 inColor;
layout(location = 1) in vec3 inPosition;
layout(location = 2) in vec3 inNorm;
layout(location = 3) in vec2 texPos;

layout(location = 0) flat out vec4 flatFragColor;
layout(location = 1) smooth out vec4 smoothFragColor;
layout(location = 2) out vec3 fragNorm;
layout(location = 3) out vec3 fragVert;
layout(location = 4) out vec2 fragTexPos;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    
    //vec4 interNorm = transpose(inverse(mat3(ubo.model))) * vec4(inNorm, 1.0f);
    
    fragNorm = transpose(inverse(mat3(ubo.model))) * vec3(inNorm);
    fragVert = transpose(inverse(mat3(ubo.model))) * vec3(inPosition);
    flatFragColor = inColor;
    smoothFragColor = inColor;
    fragTexPos = texPos;
}