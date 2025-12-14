#include "behavioral_motion_control/Boid.h"

// =============================================================================
// CONSTRUCTOR
// =============================================================================

Boid::Boid() {
    // Random initial position within world bounds
    position = Vector3D(
        (rand() / (float)RAND_MAX - 0.5f) * WORLD_SIZE * 0.8f,
        (rand() / (float)RAND_MAX - 0.5f) * WORLD_SIZE * 0.8f,
        (rand() / (float)RAND_MAX - 0.5f) * WORLD_SIZE * 0.8f
    );
    
    // Random initial velocity
    velocity = Vector3D(
        (rand() / (float)RAND_MAX - 0.5f) * 2.0f,
        (rand() / (float)RAND_MAX - 0.5f) * 2.0f,
        (rand() / (float)RAND_MAX - 0.5f) * 2.0f
    ).normalize() * (MIN_SPEED + (rand() / (float)RAND_MAX) * (MAX_SPEED - MIN_SPEED));
    
    acceleration = Vector3D();
    bankAngle = 0;
    
    // Assign color based on position using HSV color wheel
    float hue = fmod((position.x + WORLD_HALF) / WORLD_SIZE * 360.0f, 360.0f);
    hsvToRgb(hue, 0.8f, 1.0f);
}

// =============================================================================
// HSV TO RGB CONVERSION
// =============================================================================

void Boid::hsvToRgb(float h, float s, float v) {
    int i = (int)(h / 60.0f) % 6;
    float f = h / 60.0f - i;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);
    
    switch (i) {
        case 0: color = glm::vec3(v, t, p); break;
        case 1: color = glm::vec3(q, v, p); break;
        case 2: color = glm::vec3(p, v, t); break;
        case 3: color = glm::vec3(p, q, v); break;
        case 4: color = glm::vec3(t, p, v); break;
        case 5: color = glm::vec3(v, p, q); break;
    }
}

// =============================================================================
// FORCE APPLICATION
// =============================================================================

void Boid::applyForce(const Vector3D& force) {
    acceleration += force;
}

// =============================================================================
// UPDATE
// =============================================================================

void Boid::update() {
    // Calculate banking angle based on turn rate
    Vector3D turnAxis = velocity.cross(acceleration);
    float turnRate = acceleration.magnitude();
    float targetBank = turnAxis.y * turnRate * 50.0f;
    targetBank = std::max(-45.0f, std::min(45.0f, targetBank));
    bankAngle += (targetBank - bankAngle) * 0.1f;
    
    // Update velocity
    velocity += acceleration;
    
    // Clamp speed
    float speed = velocity.magnitude();
    if (speed > MAX_SPEED) {
        velocity = velocity.normalize() * MAX_SPEED;
    } else if (speed < MIN_SPEED && speed > 0) {
        velocity = velocity.normalize() * MIN_SPEED;
    }
    
    // Update position
    position += velocity;
    
    // Update trail
    trail.push_front(position);
    if (trail.size() > MAX_TRAIL_LENGTH) {
        trail.pop_back();
    }
    
    // Reset acceleration for next frame
    acceleration.zero();
}

// =============================================================================
// SEPARATION - Steer to avoid crowding local flockmates
// =============================================================================

Vector3D Boid::separate(const std::vector<Boid>& boids) {
    Vector3D steer;
    int count = 0;
    
    for (const Boid& other : boids) {
        float d = position.distanceTo(other.position);
        
        // Check if within separation radius (and not self)
        if (d > 0 && d < SEPARATION_RADIUS) {
            // Vector pointing away from neighbor, weighted by distance
            Vector3D diff = (position - other.position).normalize() / d;
            steer += diff;
            count++;
        }
    }
    
    if (count > 0) {
        steer = steer / (float)count;
        steer = steer.normalize() * MAX_SPEED - velocity;
        steer = steer.limit(MAX_FORCE);
    }
    
    return steer;
}

// =============================================================================
// ALIGNMENT - Steer towards average heading of local flockmates
// =============================================================================

Vector3D Boid::align(const std::vector<Boid>& boids) {
    Vector3D avgVelocity;
    int count = 0;
    
    for (const Boid& other : boids) {
        float d = position.distanceTo(other.position);
        
        if (d > 0 && d < ALIGNMENT_RADIUS) {
            avgVelocity += other.velocity;
            count++;
        }
    }
    
    if (count > 0) {
        avgVelocity = avgVelocity / (float)count;
        avgVelocity = avgVelocity.normalize() * MAX_SPEED;
        return (avgVelocity - velocity).limit(MAX_FORCE);
    }
    
    return Vector3D();
}

// =============================================================================
// COHESION - Steer towards center of mass of local flockmates
// =============================================================================

Vector3D Boid::cohere(const std::vector<Boid>& boids) {
    Vector3D avgPosition;
    int count = 0;
    
    for (const Boid& other : boids) {
        float d = position.distanceTo(other.position);
        
        if (d > 0 && d < COHESION_RADIUS) {
            avgPosition += other.position;
            count++;
        }
    }
    
    if (count > 0) {
        avgPosition = avgPosition / (float)count;
        return seek(avgPosition);
    }
    
    return Vector3D();
}

// =============================================================================
// SEEK - Steer towards a target position
// =============================================================================

Vector3D Boid::seek(const Vector3D& target) {
    Vector3D desired = (target - position).normalize() * MAX_SPEED;
    return (desired - velocity).limit(MAX_FORCE);
}

// =============================================================================
// FLEE - Steer away from a threat
// =============================================================================

Vector3D Boid::flee(const Vector3D& target, float radius) {
    float d = position.distanceTo(target);
    
    if (d < radius) {
        Vector3D desired = (position - target).normalize() * MAX_SPEED;
        float strength = (radius - d) / radius;
        return (desired - velocity).limit(MAX_FORCE * 2) * strength;
    }
    
    return Vector3D();
}

// =============================================================================
// AVOID OBSTACLES - Steer around spherical obstacles
// =============================================================================

Vector3D Boid::avoidObstacles(const std::vector<Obstacle>& obstacles) {
    Vector3D steer;
    
    for (const Obstacle& obs : obstacles) {
        float d = position.distanceTo(obs.position);
        float avoidDist = obs.radius + OBSTACLE_RADIUS;
        
        if (d < avoidDist) {
            Vector3D away = (position - obs.position).normalize();
            float strength = (avoidDist - d) / avoidDist;
            steer += away * strength;
        }
    }
    
    if (!steer.isZero()) {
        steer = (steer.normalize() * MAX_SPEED - velocity).limit(MAX_FORCE * 2);
    }
    
    return steer;
}

// =============================================================================
// BOUNDARY CONTAINMENT - Keep boid within world bounds
// =============================================================================

Vector3D Boid::containWithinBounds() {
    Vector3D steer;
    float margin = WORLD_HALF * 0.8f;
    float strength = 0.5f;
    
    if (position.x > margin) {
        steer.x = -strength * (position.x - margin);
    } else if (position.x < -margin) {
        steer.x = -strength * (position.x + margin);
    }
    
    if (position.y > margin) {
        steer.y = -strength * (position.y - margin);
    } else if (position.y < -margin) {
        steer.y = -strength * (position.y + margin);
    }
    
    if (position.z > margin) {
        steer.z = -strength * (position.z - margin);
    } else if (position.z < -margin) {
        steer.z = -strength * (position.z + margin);
    }
    
    return steer.limit(MAX_FORCE);
}
