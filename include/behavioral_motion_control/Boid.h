#ifndef BOID_H
#define BOID_H

#include <vector>
#include <deque>
#include <algorithm>
#include <cstdlib>
#include <glm/glm.hpp>

#include "behavioral_motion_control/Vector3D.h"
#include "behavioral_motion_control/Config.h"
#include "behavioral_motion_control/Obstacle.h"

class Boid {
public:
    Vector3D position;              // Current position
    Vector3D velocity;              // Current velocity
    Vector3D acceleration;          // Accumulated forces
    glm::vec3 color;                // Rendering color
    float bankAngle;                // Banking angle for turns
    std::deque<Vector3D> trail;     // Trail history for visualization
    
    // Constructor - initializes with random position and velocity
    Boid();
    
    // Apply a force to the boid
    void applyForce(const Vector3D& force);
    
    // Update position and velocity
    void update();
    
    // ==========================================================================
    // CORE FLOCKING BEHAVIORS (Reynolds' Algorithm)
    // ==========================================================================
    
    // SEPARATION: Steer to avoid crowding local flockmates
    Vector3D separate(const std::vector<Boid>& boids);
    
    // ALIGNMENT: Steer towards average heading of local flockmates
    Vector3D align(const std::vector<Boid>& boids);
    
    // COHESION: Steer towards average position of local flockmates
    Vector3D cohere(const std::vector<Boid>& boids);
    
    // ==========================================================================
    // ADDITIONAL BEHAVIORS
    // ==========================================================================
    
    // SEEK: Steer towards a target position
    Vector3D seek(const Vector3D& target);
    
    // FLEE: Steer away from a threat
    Vector3D flee(const Vector3D& target, float radius);
    
    // AVOID OBSTACLES: Steer around spherical obstacles
    Vector3D avoidObstacles(const std::vector<Obstacle>& obstacles);
    
    // BOUNDARY: Keep boid within world bounds
    Vector3D containWithinBounds();
    
private:
    // Convert HSV to RGB for colorful boid assignment
    void hsvToRgb(float h, float s, float v);
};

#endif // BOID_H
