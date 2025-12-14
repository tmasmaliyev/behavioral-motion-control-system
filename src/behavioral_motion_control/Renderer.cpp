#include "behavioral_motion_control/Renderer.h"
#include "behavioral_motion_control/Config.h"
#include <iostream>
#include <vector>

// =============================================================================
// SHADER COMPILATION
// =============================================================================

GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "Shader compilation error: " << infoLog << std::endl;
    }
    return shader;
}

GLuint createShaderProgram(const char* vertexSrc, const char* fragmentSrc) {
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSrc);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSrc);
    
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    
    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "Shader linking error: " << infoLog << std::endl;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return program;
}

// =============================================================================
// GEOMETRY CREATION - CONE (for boids)
// =============================================================================

void createConeGeometry(GLuint& vao, GLuint& vbo, GLuint& ebo, int& indexCount) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    
    const int segments = 16;
    const float height = BOID_SIZE * 2.0f;
    const float radius = BOID_SIZE * 0.5f;
    
    // Tip of cone (vertex 0)
    vertices.push_back(0); vertices.push_back(height); vertices.push_back(0);
    vertices.push_back(0); vertices.push_back(1); vertices.push_back(0);
    
    // Base vertices (vertices 1 to segments+1)
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * M_PI * i / segments;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        
        vertices.push_back(x);
        vertices.push_back(0);
        vertices.push_back(z);
        
        // Normal pointing outward and up
        glm::vec3 normal = glm::normalize(glm::vec3(x, radius / height, z));
        vertices.push_back(normal.x);
        vertices.push_back(normal.y);
        vertices.push_back(normal.z);
    }
    
    // Center of base
    int baseCenterIndex = 1 + segments + 1;
    vertices.push_back(0); vertices.push_back(0); vertices.push_back(0);
    vertices.push_back(0); vertices.push_back(-1); vertices.push_back(0);
    
    // Side triangles
    for (int i = 0; i < segments; i++) {
        indices.push_back(0);
        indices.push_back(1 + i);
        indices.push_back(1 + i + 1);
    }
    
    // Base triangles
    for (int i = 0; i < segments; i++) {
        indices.push_back(baseCenterIndex);
        indices.push_back(1 + i + 1);
        indices.push_back(1 + i);
    }
    
    indexCount = indices.size();
    
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    
    glBindVertexArray(vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

// =============================================================================
// GEOMETRY CREATION - SPHERE (for obstacles, predator, goal)
// =============================================================================

void createSphereGeometry(GLuint& vao, GLuint& vbo, GLuint& ebo, int& indexCount) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    
    const int stacks = 16;
    const int slices = 16;
    
    // Generate vertices
    for (int i = 0; i <= stacks; i++) {
        float phi = M_PI * i / stacks;
        for (int j = 0; j <= slices; j++) {
            float theta = 2.0f * M_PI * j / slices;
            
            float x = sin(phi) * cos(theta);
            float y = cos(phi);
            float z = sin(phi) * sin(theta);
            
            // Position
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            
            // Normal (same as position for unit sphere)
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }
    
    // Generate indices
    for (int i = 0; i < stacks; i++) {
        for (int j = 0; j < slices; j++) {
            int first = i * (slices + 1) + j;
            int second = first + slices + 1;
            
            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);
            
            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }
    
    indexCount = indices.size();
    
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    
    glBindVertexArray(vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

// =============================================================================
// GEOMETRY CREATION - LINE BUFFERS (for trails, wireframe)
// =============================================================================

void createLineBuffers(GLuint& vao, GLuint& vbo) {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
    // Position (3 floats) + Color (4 floats)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

// =============================================================================
// DRAW SPHERE
// =============================================================================

void drawSphere(GLuint shaderProgram, GLuint sphereVAO, int sphereIndexCount,
                const glm::vec3& position, float radius, const glm::vec3& color,
                const glm::mat4& view, const glm::mat4& projection, 
                const glm::vec3& viewPos) {
    glUseProgram(shaderProgram);
    
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    model = glm::scale(model, glm::vec3(radius));
    
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(glGetUniformLocation(shaderProgram, "objectColor"), 1, glm::value_ptr(color));
    glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(viewPos));
    glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), 50.0f, 100.0f, 50.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "lightPos2"), -30.0f, -50.0f, -30.0f);
    glUniform1f(glGetUniformLocation(shaderProgram, "ambient"), 0.3f);
    glUniform1f(glGetUniformLocation(shaderProgram, "specularStrength"), 0.5f);
    
    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// =============================================================================
// DRAW BOID
// =============================================================================

void drawBoid(GLuint shaderProgram, GLuint coneVAO, int coneIndexCount,
              const Boid& boid, bool showBanking,
              const glm::mat4& view, const glm::mat4& projection,
              const glm::vec3& viewPos) {
    glUseProgram(shaderProgram);
    
    // Calculate rotation to face velocity direction
    glm::vec3 vel = boid.velocity.toGLM();
    float len = glm::length(vel);
    glm::vec3 dir = len > 0.001f ? glm::normalize(vel) : glm::vec3(0, 1, 0);
    glm::vec3 up(0, 1, 0);
    
    glm::mat4 model = glm::translate(glm::mat4(1.0f), boid.position.toGLM());
    
    // Rotate to align with velocity
    float angle = acos(glm::clamp(glm::dot(dir, up), -1.0f, 1.0f));
    glm::vec3 axis = glm::cross(up, dir);
    if (glm::length(axis) > 0.001f) {
        axis = glm::normalize(axis);
        model = glm::rotate(model, angle, axis);
    }
    
    // Apply banking
    if (showBanking) {
        model = glm::rotate(model, glm::radians(boid.bankAngle), glm::vec3(0, 1, 0));
    }
    
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(glGetUniformLocation(shaderProgram, "objectColor"), 1, glm::value_ptr(boid.color));
    glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(viewPos));
    glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), 50.0f, 100.0f, 50.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "lightPos2"), -30.0f, -50.0f, -30.0f);
    glUniform1f(glGetUniformLocation(shaderProgram, "ambient"), 0.3f);
    glUniform1f(glGetUniformLocation(shaderProgram, "specularStrength"), 0.8f);
    
    glBindVertexArray(coneVAO);
    glDrawElements(GL_TRIANGLES, coneIndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// =============================================================================
// DRAW TRAILS
// =============================================================================

void drawTrails(GLuint lineShaderProgram, GLuint lineVAO, GLuint lineVBO,
                const std::vector<Boid>& boids, const Predator& predator,
                bool predatorEnabled, bool showTrails,
                const glm::mat4& view, const glm::mat4& projection) {
    if (!showTrails) return;
    
    std::vector<float> lineData;
    
    // Boid trails
    for (const Boid& boid : boids) {
        if (boid.trail.size() < 2) continue;
        
        for (size_t i = 0; i < boid.trail.size() - 1; i++) {
            float alpha = 1.0f - (float)i / boid.trail.size();
            
            // Start point
            lineData.push_back(boid.trail[i].x);
            lineData.push_back(boid.trail[i].y);
            lineData.push_back(boid.trail[i].z);
            lineData.push_back(boid.color.r);
            lineData.push_back(boid.color.g);
            lineData.push_back(boid.color.b);
            lineData.push_back(alpha * 0.5f);
            
            // End point
            lineData.push_back(boid.trail[i + 1].x);
            lineData.push_back(boid.trail[i + 1].y);
            lineData.push_back(boid.trail[i + 1].z);
            lineData.push_back(boid.color.r);
            lineData.push_back(boid.color.g);
            lineData.push_back(boid.color.b);
            lineData.push_back(alpha * 0.3f);
        }
    }
    
    // Predator trail
    if (predatorEnabled && predator.trail.size() > 1) {
        for (size_t i = 0; i < predator.trail.size() - 1; i++) {
            float alpha = 1.0f - (float)i / predator.trail.size();
            
            lineData.push_back(predator.trail[i].x);
            lineData.push_back(predator.trail[i].y);
            lineData.push_back(predator.trail[i].z);
            lineData.push_back(0.9f);
            lineData.push_back(0.1f);
            lineData.push_back(0.1f);
            lineData.push_back(alpha * 0.5f);
            
            lineData.push_back(predator.trail[i + 1].x);
            lineData.push_back(predator.trail[i + 1].y);
            lineData.push_back(predator.trail[i + 1].z);
            lineData.push_back(0.9f);
            lineData.push_back(0.1f);
            lineData.push_back(0.1f);
            lineData.push_back(alpha * 0.3f);
        }
    }
    
    if (lineData.empty()) return;
    
    glUseProgram(lineShaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(lineShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(lineShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    
    glBindVertexArray(lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferData(GL_ARRAY_BUFFER, lineData.size() * sizeof(float), lineData.data(), GL_DYNAMIC_DRAW);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawArrays(GL_LINES, 0, lineData.size() / 7);
    glDisable(GL_BLEND);
    
    glBindVertexArray(0);
}

// =============================================================================
// DRAW WORLD BOUNDS
// =============================================================================

void drawWorldBounds(GLuint lineShaderProgram, GLuint lineVAO, GLuint lineVBO,
                     const glm::mat4& view, const glm::mat4& projection) {
    std::vector<float> lineData;
    float s = WORLD_HALF;
    
    // 8 corners of the cube
    glm::vec3 corners[8] = {
        {-s, -s, -s}, {s, -s, -s}, {s, s, -s}, {-s, s, -s},
        {-s, -s, s}, {s, -s, s}, {s, s, s}, {-s, s, s}
    };
    
    // 12 edges of the cube
    int edges[12][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        {4, 5}, {5, 6}, {6, 7}, {7, 4},
        {0, 4}, {1, 5}, {2, 6}, {3, 7}
    };
    
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 2; j++) {
            lineData.push_back(corners[edges[i][j]].x);
            lineData.push_back(corners[edges[i][j]].y);
            lineData.push_back(corners[edges[i][j]].z);
            lineData.push_back(0.3f);  // R
            lineData.push_back(0.3f);  // G
            lineData.push_back(0.5f);  // B
            lineData.push_back(0.5f);  // A
        }
    }
    
    glUseProgram(lineShaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(lineShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(lineShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    
    glBindVertexArray(lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferData(GL_ARRAY_BUFFER, lineData.size() * sizeof(float), lineData.data(), GL_DYNAMIC_DRAW);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawArrays(GL_LINES, 0, lineData.size() / 7);
    glDisable(GL_BLEND);
    
    glBindVertexArray(0);
}
