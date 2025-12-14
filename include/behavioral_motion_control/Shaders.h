#ifndef SHADERS_H
#define SHADERS_H

// =============================================================================
// MAIN OBJECT VERTEX SHADER
// Transforms vertices and passes normals for lighting calculations
// =============================================================================

const char* const vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

// =============================================================================
// MAIN OBJECT FRAGMENT SHADER
// Implements Phong lighting with two light sources
// =============================================================================

const char* const fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightPos;
uniform vec3 lightPos2;
uniform vec3 viewPos;
uniform vec3 objectColor;
uniform float ambient;
uniform float specularStrength;

void main() {
    // Ambient lighting
    vec3 ambientColor = ambient * objectColor;
    
    // Diffuse lighting (main light)
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * objectColor * 0.7;
    
    // Diffuse lighting (fill light)
    vec3 lightDir2 = normalize(lightPos2 - FragPos);
    float diff2 = max(dot(norm, lightDir2), 0.0);
    vec3 diffuse2 = diff2 * objectColor * 0.3;
    
    // Specular lighting
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = specularStrength * spec * vec3(1.0);
    
    // Combined result
    vec3 result = ambientColor + diffuse + diffuse2 + specular;
    FragColor = vec4(result, 1.0);
}
)";

// =============================================================================
// LINE VERTEX SHADER
// Simple pass-through for trail and wireframe rendering
// =============================================================================

const char* const lineVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;

out vec4 vertexColor;

uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * vec4(aPos, 1.0);
    vertexColor = aColor;
}
)";

// =============================================================================
// LINE FRAGMENT SHADER
// Outputs vertex color with alpha for transparency
// =============================================================================

const char* const lineFragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
in vec4 vertexColor;

void main() {
    FragColor = vertexColor;
}
)";

#endif // SHADERS_H
