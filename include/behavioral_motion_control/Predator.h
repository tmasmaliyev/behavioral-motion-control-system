#ifndef PREDATOR_H
#define PREDATOR_H

#include <vector>
#include <deque>
#include <cstdlib>

#include "behavioral_motion_control/Vector3D.h"
#include "behavioral_motion_control/Config.h"
#include "behavioral_motion_control/Boid.h"

class Predator {
public:
    Vector3D position;              // Current position
    Vector3D velocity;              // Current velocity
    float size;                     // Rendering size
    std::deque<Vector3D> trail;     // Trail history
    
    // Constructor
    Predator();
    
    // Update predator - chases nearest boid
    void update(const std::vector<Boid>& boids);
};

#endif // PREDATOR_H
