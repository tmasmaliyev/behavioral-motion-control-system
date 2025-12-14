#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "behavioral_motion_control/Boid.h"
#include "behavioral_motion_control/Predator.h"
#include "behavioral_motion_control/Obstacle.h"

// =============================================================================
// SHADER MANAGEMENT
// =============================================================================

// Compile a single shader
GLuint compileShader(GLenum type, const char* source);

// Create a shader program from vertex and fragment shaders
GLuint createShaderProgram(const char* vertexSrc, const char* fragmentSrc);

// =============================================================================
// GEOMETRY CREATION
// =============================================================================

// Create cone geometry for boids (VAO, VBO, EBO)
void createConeGeometry(GLuint& vao, GLuint& vbo, GLuint& ebo, int& indexCount);

// Create sphere geometry for obstacles/predator (VAO, VBO, EBO)
void createSphereGeometry(GLuint& vao, GLuint& vbo, GLuint& ebo, int& indexCount);

// Create line buffers for trails and wireframes
void createLineBuffers(GLuint& vao, GLuint& vbo);

// =============================================================================
// DRAWING FUNCTIONS
// =============================================================================

// Draw a sphere at given position
void drawSphere(GLuint shaderProgram, GLuint sphereVAO, int sphereIndexCount,
                const glm::vec3& position, float radius, const glm::vec3& color,
                const glm::mat4& view, const glm::mat4& projection, 
                const glm::vec3& viewPos);

// Draw a single boid
void drawBoid(GLuint shaderProgram, GLuint coneVAO, int coneIndexCount,
              const Boid& boid, bool showBanking,
              const glm::mat4& view, const glm::mat4& projection,
              const glm::vec3& viewPos);

// Draw all trails
void drawTrails(GLuint lineShaderProgram, GLuint lineVAO, GLuint lineVBO,
                const std::vector<Boid>& boids, const Predator& predator,
                bool predatorEnabled, bool showTrails,
                const glm::mat4& view, const glm::mat4& projection);

// Draw world boundary wireframe
void drawWorldBounds(GLuint lineShaderProgram, GLuint lineVAO, GLuint lineVBO,
                     const glm::mat4& view, const glm::mat4& projection);

#endif // RENDERER_H
