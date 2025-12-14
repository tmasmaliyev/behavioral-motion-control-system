#include "behavioral_motion_control/Predator.h"

// =============================================================================
// CONSTRUCTOR
// =============================================================================

Predator::Predator() {
    position = Vector3D(0, 0, 0);
    
    // Random initial velocity
    velocity = Vector3D(
        (rand() / (float)RAND_MAX - 0.5f),
        (rand() / (float)RAND_MAX - 0.5f),
        (rand() / (float)RAND_MAX - 0.5f)
    ).normalize() * 1.0f;
    
    size = BOID_SIZE * 3.0f;
}

// =============================================================================
// UPDATE - Chase nearest boid
// =============================================================================

void Predator::update(const std::vector<Boid>& boids) {
    // Find nearest boid
    float nearestDist = WORLD_SIZE * 2;
    Vector3D nearestPos = position;
    
    for (const Boid& boid : boids) {
        float d = position.distanceTo(boid.position);
        if (d < nearestDist) {
            nearestDist = d;
            nearestPos = boid.position;
        }
    }
    
    // Steer towards nearest boid (slightly slower than boids)
    Vector3D desired = (nearestPos - position).normalize() * (MAX_SPEED * 0.7f);
    Vector3D steer = (desired - velocity).limit(MAX_FORCE * 0.5f);
    velocity = (velocity + steer).limit(MAX_SPEED * 0.8f);
    
    // Boundary containment
    float margin = WORLD_HALF * 0.9f;
    if (position.x > margin) velocity.x -= 0.1f;
    if (position.x < -margin) velocity.x += 0.1f;
    if (position.y > margin) velocity.y -= 0.1f;
    if (position.y < -margin) velocity.y += 0.1f;
    if (position.z > margin) velocity.z -= 0.1f;
    if (position.z < -margin) velocity.z += 0.1f;
    
    // Update position
    position += velocity;
    
    // Update trail
    trail.push_front(position);
    if (trail.size() > MAX_TRAIL_LENGTH * 2) {
        trail.pop_back();
    }
}
