#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <cstdlib>
#include <glm/glm.hpp>
#include "behavioral_motion_control/Vector3D.h"

class Obstacle {
public:
    Vector3D position;  // Center position
    float radius;       // Sphere radius
    glm::vec3 color;    // Rendering color
    
    // Constructor
    Obstacle(Vector3D pos, float rad) : position(pos), radius(rad) {
        // Random dark color for obstacle
        color = glm::vec3(
            0.3f + (rand() % 30) / 100.0f,
            0.3f + (rand() % 30) / 100.0f,
            0.3f + (rand() % 30) / 100.0f
        );
    }
};

#endif // OBSTACLE_H
