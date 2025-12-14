#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

#include "behavioral_motion_control/Config.h"
#include "behavioral_motion_control/Shaders.h"
#include "behavioral_motion_control/Vector3D.h"
#include "behavioral_motion_control/Boid.h"
#include "behavioral_motion_control/Predator.h"
#include "behavioral_motion_control/Obstacle.h"
#include "behavioral_motion_control/Renderer.h"

// =============================================================================
// GLOBAL VARIABLES - Behavior Weights (defined in Config.h as extern)
// =============================================================================

float separationWeight = 1.5f;
float alignmentWeight = 1.0f;
float cohesionWeight = 1.0f;
float obstacleWeight = 2.0f;
float predatorWeight = 3.0f;
float goalWeight = 0.5f;
float boundaryWeight = 1.5f;

// =============================================================================
// OPENGL RESOURCES
// =============================================================================

GLFWwindow* window = nullptr;
GLuint shaderProgram, lineShaderProgram;
GLuint coneVAO, coneVBO, coneEBO;
GLuint sphereVAO, sphereVBO, sphereEBO;
GLuint lineVAO, lineVBO;
int coneIndexCount = 0;
int sphereIndexCount = 0;

// =============================================================================
// CAMERA
// =============================================================================

float cameraAngleX = 30.0f;
float cameraAngleY = 45.0f;
float cameraDistance = 150.0f;

// =============================================================================
// SIMULATION STATE
// =============================================================================

bool paused = false;
bool showTrails = true;
bool showBanking = true;

bool separationEnabled = true;
bool alignmentEnabled = true;
bool cohesionEnabled = true;
bool obstaclesEnabled = true;
bool predatorEnabled = true;
bool goalEnabled = false;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// =============================================================================
// SIMULATION OBJECTS
// =============================================================================

std::vector<Boid> boids;
std::vector<Obstacle> obstacles;
Predator predator;
Vector3D goal;

// =============================================================================
// INITIALIZATION FUNCTIONS
// =============================================================================

void initBoids() {
    boids.clear();
    for (int i = 0; i < INITIAL_BOID_COUNT; i++) {
        boids.push_back(Boid());
    }
}

void initObstacles() {
    obstacles.clear();
    obstacles.push_back(Obstacle(Vector3D(20, 0, 0), 8));
    obstacles.push_back(Obstacle(Vector3D(-20, 10, 15), 6));
    obstacles.push_back(Obstacle(Vector3D(0, -15, -20), 7));
    obstacles.push_back(Obstacle(Vector3D(-25, 5, -10), 5));
    obstacles.push_back(Obstacle(Vector3D(15, 20, 10), 6));
}

void initGoal() {
    goal = Vector3D(
        (rand() / (float)RAND_MAX - 0.5f) * WORLD_SIZE * 0.6f,
        (rand() / (float)RAND_MAX - 0.5f) * WORLD_SIZE * 0.6f,
        (rand() / (float)RAND_MAX - 0.5f) * WORLD_SIZE * 0.6f
    );
}

// =============================================================================
// SIMULATION UPDATE
// =============================================================================

void updateSimulation() {
    if (paused) return;
    
    // Update predator
    if (predatorEnabled) {
        predator.update(boids);
    }
    
    // Occasionally move goal
    if (goalEnabled && rand() % 500 == 0) {
        initGoal();
    }
    
    // Update each boid
    for (Boid& boid : boids) {
        Vector3D force;
        
        // Core flocking behaviors
        if (separationEnabled) force += boid.separate(boids) * separationWeight;
        if (alignmentEnabled) force += boid.align(boids) * alignmentWeight;
        if (cohesionEnabled) force += boid.cohere(boids) * cohesionWeight;
        
        // Extensions
        if (obstaclesEnabled) force += boid.avoidObstacles(obstacles) * obstacleWeight;
        if (predatorEnabled) force += boid.flee(predator.position, PREDATOR_RADIUS) * predatorWeight;
        if (goalEnabled) force += boid.seek(goal) * goalWeight;
        
        // Boundary containment
        force += boid.containWithinBounds() * boundaryWeight;
        
        boid.applyForce(force);
        boid.update();
    }
}

// =============================================================================
// INPUT CALLBACKS
// =============================================================================

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS && action != GLFW_REPEAT) return;
    
    switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, true);
            break;
        case GLFW_KEY_P:
            paused = !paused;
            std::cout << (paused ? "Paused" : "Resumed") << std::endl;
            break;
        case GLFW_KEY_R:
            initBoids();
            predator = Predator();
            initGoal();
            std::cout << "Simulation reset" << std::endl;
            break;
        case GLFW_KEY_T:
            showTrails = !showTrails;
            std::cout << "Trails: " << (showTrails ? "ON" : "OFF") << std::endl;
            break;
        case GLFW_KEY_B:
            showBanking = !showBanking;
            std::cout << "Banking: " << (showBanking ? "ON" : "OFF") << std::endl;
            break;
        case GLFW_KEY_SPACE:
            for (int i = 0; i < 10; i++) boids.push_back(Boid());
            std::cout << "Added 10 boids. Total: " << boids.size() << std::endl;
            break;
        case GLFW_KEY_1:
            separationEnabled = !separationEnabled;
            std::cout << "Separation: " << (separationEnabled ? "ON" : "OFF") << std::endl;
            break;
        case GLFW_KEY_2:
            alignmentEnabled = !alignmentEnabled;
            std::cout << "Alignment: " << (alignmentEnabled ? "ON" : "OFF") << std::endl;
            break;
        case GLFW_KEY_3:
            cohesionEnabled = !cohesionEnabled;
            std::cout << "Cohesion: " << (cohesionEnabled ? "ON" : "OFF") << std::endl;
            break;
        case GLFW_KEY_4:
            obstaclesEnabled = !obstaclesEnabled;
            std::cout << "Obstacles: " << (obstaclesEnabled ? "ON" : "OFF") << std::endl;
            break;
        case GLFW_KEY_5:
            predatorEnabled = !predatorEnabled;
            std::cout << "Predator: " << (predatorEnabled ? "ON" : "OFF") << std::endl;
            break;
        case GLFW_KEY_6:
            goalEnabled = !goalEnabled;
            std::cout << "Goal: " << (goalEnabled ? "ON" : "OFF") << std::endl;
            break;
        case GLFW_KEY_UP:
            cameraAngleX = std::min(89.0f, cameraAngleX + 5.0f);
            break;
        case GLFW_KEY_DOWN:
            cameraAngleX = std::max(-89.0f, cameraAngleX - 5.0f);
            break;
        case GLFW_KEY_LEFT:
            cameraAngleY -= 5.0f;
            break;
        case GLFW_KEY_RIGHT:
            cameraAngleY += 5.0f;
            break;
        case GLFW_KEY_EQUAL:
        case GLFW_KEY_KP_ADD:
            cameraDistance = std::max(50.0f, cameraDistance - 10.0f);
            break;
        case GLFW_KEY_MINUS:
        case GLFW_KEY_KP_SUBTRACT:
            cameraDistance = std::min(300.0f, cameraDistance + 10.0f);
            break;
    }
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    // Configure GLFW for OpenGL 3.3 Core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);  // Anti-aliasing
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    // Create window
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    
    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }
    
    // OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glClearColor(0.05f, 0.05f, 0.15f, 1.0f);
    
    // Create shaders
    shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    lineShaderProgram = createShaderProgram(lineVertexShaderSource, lineFragmentShaderSource);
    
    // Create geometry
    createConeGeometry(coneVAO, coneVBO, coneEBO, coneIndexCount);
    createSphereGeometry(sphereVAO, sphereVBO, sphereEBO, sphereIndexCount);
    createLineBuffers(lineVAO, lineVBO);
    
    // Initialize simulation
    srand(time(NULL));
    initBoids();
    initObstacles();
    initGoal();
    predator = Predator();
    
    // Print controls
    std::cout << "============================================" << std::endl;
    std::cout << "  BOIDS FLOCKING SIMULATION" << std::endl;
    std::cout << "  Reynolds' Algorithm - Modern OpenGL" << std::endl;
    std::cout << "============================================" << std::endl;
    std::cout << std::endl;
    std::cout << "CONTROLS:" << std::endl;
    std::cout << "  Arrow keys: Rotate camera" << std::endl;
    std::cout << "  +/-: Zoom in/out" << std::endl;
    std::cout << "  1: Toggle Separation" << std::endl;
    std::cout << "  2: Toggle Alignment" << std::endl;
    std::cout << "  3: Toggle Cohesion" << std::endl;
    std::cout << "  4: Toggle Obstacles" << std::endl;
    std::cout << "  5: Toggle Predator" << std::endl;
    std::cout << "  6: Toggle Goal" << std::endl;
    std::cout << "  P: Pause/Resume" << std::endl;
    std::cout << "  R: Reset" << std::endl;
    std::cout << "  T: Toggle trails" << std::endl;
    std::cout << "  B: Toggle banking" << std::endl;
    std::cout << "  SPACE: Add boids" << std::endl;
    std::cout << "  ESC: Exit" << std::endl;
    std::cout << "============================================" << std::endl;
    
    // Main render loop
    while (!glfwWindowShouldClose(window)) {
        // Calculate delta time
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Update simulation
        updateSimulation();
        
        // Clear buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Calculate camera position
        float camX = cameraDistance * sin(glm::radians(cameraAngleY)) * cos(glm::radians(cameraAngleX));
        float camY = cameraDistance * sin(glm::radians(cameraAngleX));
        float camZ = cameraDistance * cos(glm::radians(cameraAngleY)) * cos(glm::radians(cameraAngleX));
        glm::vec3 cameraPos(camX, camY, camZ);
        
        // Create view matrix
        glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        
        // Create projection matrix
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f), 
            (float)width / (float)height, 
            1.0f, 500.0f
        );
        
        // Draw world bounds
        drawWorldBounds(lineShaderProgram, lineVAO, lineVBO, view, projection);
        
        // Draw obstacles
        if (obstaclesEnabled) {
            for (const Obstacle& obs : obstacles) {
                drawSphere(shaderProgram, sphereVAO, sphereIndexCount,
                          obs.position.toGLM(), obs.radius, obs.color,
                          view, projection, cameraPos);
            }
        }
        
        // Draw goal
        if (goalEnabled) {
            float pulse = 0.5f + 0.5f * sin(currentFrame * 5.0f);
            drawSphere(shaderProgram, sphereVAO, sphereIndexCount,
                      goal.toGLM(), 2.0f + pulse, glm::vec3(1.0f, 0.85f, 0.0f),
                      view, projection, cameraPos);
        }
        
        // Draw predator
        if (predatorEnabled) {
            drawSphere(shaderProgram, sphereVAO, sphereIndexCount,
                      predator.position.toGLM(), predator.size, glm::vec3(0.9f, 0.1f, 0.1f),
                      view, projection, cameraPos);
        }
        
        // Draw boids
        for (const Boid& boid : boids) {
            drawBoid(shaderProgram, coneVAO, coneIndexCount,
                    boid, showBanking, view, projection, cameraPos);
        }
        
        // Draw trails
        drawTrails(lineShaderProgram, lineVAO, lineVBO,
                  boids, predator, predatorEnabled, showTrails,
                  view, projection);
        
        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // Cleanup
    glDeleteVertexArrays(1, &coneVAO);
    glDeleteVertexArrays(1, &sphereVAO);
    glDeleteVertexArrays(1, &lineVAO);
    glDeleteBuffers(1, &coneVBO);
    glDeleteBuffers(1, &coneEBO);
    glDeleteBuffers(1, &sphereVBO);
    glDeleteBuffers(1, &sphereEBO);
    glDeleteBuffers(1, &lineVBO);
    glDeleteProgram(shaderProgram);
    glDeleteProgram(lineShaderProgram);
    
    glfwTerminate();
    return 0;
}
