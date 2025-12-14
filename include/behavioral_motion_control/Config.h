#ifndef CONFIG_H
#define CONFIG_H

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// =============================================================================
// WINDOW SETTINGS
// =============================================================================

const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;
const char* const WINDOW_TITLE = "Boids Flocking Simulation - Reynolds Algorithm";

// =============================================================================
// WORLD SETTINGS
// =============================================================================

const float WORLD_SIZE = 100.0f;
const float WORLD_HALF = WORLD_SIZE / 2.0f;

// =============================================================================
// BOID PARAMETERS
// =============================================================================

const int INITIAL_BOID_COUNT = 100;
const float BOID_SIZE = 1.5f;
const float MAX_SPEED = 2.0f;
const float MIN_SPEED = 0.5f;
const float MAX_FORCE = 0.1f;

// =============================================================================
// BEHAVIOR RADII
// =============================================================================

const float SEPARATION_RADIUS = 5.0f;
const float ALIGNMENT_RADIUS = 15.0f;
const float COHESION_RADIUS = 20.0f;
const float OBSTACLE_RADIUS = 15.0f;
const float PREDATOR_RADIUS = 25.0f;

// =============================================================================
// BEHAVIOR WEIGHTS (can be modified at runtime)
// =============================================================================

extern float separationWeight;
extern float alignmentWeight;
extern float cohesionWeight;
extern float obstacleWeight;
extern float predatorWeight;
extern float goalWeight;
extern float boundaryWeight;

// =============================================================================
// TRAIL SETTINGS
// =============================================================================

const int MAX_TRAIL_LENGTH = 30;

#endif // CONFIG_H
