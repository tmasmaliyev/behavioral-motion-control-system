# Behavioral Motion Control System (Boids Flocking Simulation)

A modern OpenGL-based flocking simulation system implemented in C++ for computer animation coursework. Features Craig Reynolds' Boids algorithm with separation, alignment, cohesion, and advanced extensions.

## Table of Contents

- [Description](#description)
- [Features](#features)
- [Requirements](#requirements)
- [How to Build](#how-to-build)
  - [Linux](#linux)
  - [Windows](#windows)
- [How to Use](#how-to-use)
  - [Interactive Controls](#interactive-controls)
- [Project Structure](#project-structure)
- [Technical Details](#technical-details)
- [Configuration](#configuration)
- [Performance Notes](#performance-notes)

## Description

This project implements Craig Reynolds' **Boids** algorithm for simulating the flocking behavior of birds, fish, or other creatures. It demonstrates fundamental behavioral animation concepts including:

- **Separation** - Steer to avoid crowding local flockmates
- **Alignment** - Steer towards average heading of local flockmates
- **Cohesion** - Steer towards average position of local flockmates
- **Obstacle avoidance** - Navigate around spherical obstacles
- **Predator evasion** - Flee from threatening entities
- **Goal seeking** - Move towards target positions
- **Interactive 3D visualization** with camera controls

The system simulates multiple autonomous agents (boids) exhibiting emergent flocking behavior through simple local rules, creating complex, lifelike group dynamics.

## Features

### Core Behaviors (Reynolds' Algorithm)
- **Separation**: Boids steer away from neighbors within separation radius
- **Alignment**: Boids match velocity with nearby flockmates
- **Cohesion**: Boids steer toward center of mass of local group
- **Weighted Combination**: Behaviors combined via configurable weights

### Advanced Extensions
- **Obstacle Avoidance**: Spherical obstacles with distance-based steering
- **Predator Evasion**: Red predator entity that chases nearest boid
- **Goal Seeking**: Golden target sphere that attracts the flock
- **Banking Visualization**: Boids bank/roll based on turn rate
- **Trail Rendering**: Fading trails showing movement history
- **Boundary Containment**: Soft boundaries keep boids in world space

### Rendering & Interaction
- **Modern OpenGL 3.3**: Shader-based rendering with Phong lighting
- **Interactive Camera**: Keyboard-controlled rotation and zoom
- **Environment Visualization**: Wireframe world bounds
- **Runtime Behavior Toggles**: Enable/disable behaviors on-the-fly
- **Dynamic Boid Addition**: Add more boids during simulation
- **Cross-platform**: Supports Linux, and Windows

## Requirements

### Build Dependencies
- **GNU Make** or **CMake** (3.10+)
- **C++ Compiler** supporting C++11 (GCC 4.8+, Clang 3.3+, MSVC 2015+)

### Runtime Libraries
- **OpenGL** 3.3+ (Core Profile)
- **GLFW3** (window management and input)
- **GLEW** (OpenGL extension loading)
- **GLM** (OpenGL Mathematics - header-only)

### Platform-Specific
- **Linux**: Mesa OpenGL, X11 development libraries
- **Windows**: Visual Studio 2015+ or MinGW-w64

## How to Build

### Linux

**Install dependencies (Fedora):**
```bash
sudo dnf install gcc-c++ glfw-devel glew-devel glm-devel
```

**Build the project:**
```bash
# Using CMake
mkdir build
cmake --preset linux-vcpkg
cmake --build build --config Release
./behavioral_motion_control
```

# Run
```
./behavioral_motion_control
```

### Windows

# Build
```
mkdir build
cmake --preset linux-vcpkg
cmake --build build --config Release
```

# Run
```
./behavioral_motion_control.exe
```

**Using Visual Studio:**
1. Install GLEW, GLFW, and GLM via vcpkg
2. Create a new project and add all source files
3. Configure include directories and link libraries
4. Build and run

## How to Use

### Interactive Controls

#### Keyboard Controls
| Key | Action |
|-----|--------|
| **ESC** | Exit application |
| **P** | Pause/Resume simulation |
| **R** | Reset simulation |
| **T** | Toggle trail visualization |
| **B** | Toggle banking visualization |
| **SPACE** | Add 10 more boids |
| **1** | Toggle Separation behavior |
| **2** | Toggle Alignment behavior |
| **3** | Toggle Cohesion behavior |
| **4** | Toggle Obstacle avoidance |
| **5** | Toggle Predator |
| **6** | Toggle Goal seeking |

#### Camera Controls
| Key | Action |
|-----|--------|
| **UP** | Rotate camera up |
| **DOWN** | Rotate camera down |
| **LEFT** | Rotate camera left |
| **RIGHT** | Rotate camera right |
| **+/=** | Zoom in |
| **-** | Zoom out |


## Technical Details

### Flocking Algorithm

**Separation:**
```
For each nearby boid within SEPARATION_RADIUS:
    diff = position - neighbor.position
    diff = normalize(diff) / distance
    steer += diff
steer = normalize(steer) * MAX_SPEED - velocity
steer = limit(steer, MAX_FORCE)
```

**Alignment:**
```
For each nearby boid within ALIGNMENT_RADIUS:
    avgVelocity += neighbor.velocity
avgVelocity = avgVelocity / count
avgVelocity = normalize(avgVelocity) * MAX_SPEED
steer = avgVelocity - velocity
steer = limit(steer, MAX_FORCE)
```

**Cohesion:**
```
For each nearby boid within COHESION_RADIUS:
    avgPosition += neighbor.position
avgPosition = avgPosition / count
steer = seek(avgPosition)
```

**Force Combination (Weighted Average):**
```
totalForce = separation * 1.5 +
             alignment  * 1.0 +
             cohesion   * 1.0 +
             obstacles  * 2.0 +
             predator   * 3.0 +
             goal       * 0.5 +
             boundary   * 1.5
```

### Rendering Pipeline

**Shaders (GLSL 3.30):**
- Vertex Shader: Model-View-Projection transform, normal transformation
- Fragment Shader: Phong lighting with 2 light sources (ambient + diffuse + specular)

**Geometry:**
- Boids: Cone primitives oriented along velocity vector
- Obstacles/Predator/Goal: Sphere primitives
- Trails: Line primitives with alpha blending
- World bounds: Wireframe cube

### Banking Calculation

```cpp
Vector3D turnAxis = velocity.cross(acceleration);
float turnRate = acceleration.magnitude();
float targetBank = turnAxis.y * turnRate * 50.0f;
targetBank = clamp(targetBank, -45.0f, 45.0f);
bankAngle += (targetBank - bankAngle) * 0.1f;  // Smooth interpolation
```

## Configuration

Key parameters can be modified in `include/Config.h`:

### World Settings
| Parameter | Default | Description |
|-----------|---------|-------------|
| `WORLD_SIZE` | 100.0 | Size of simulation space |
| `INITIAL_BOID_COUNT` | 100 | Starting number of boids |

### Boid Parameters
| Parameter | Default | Description |
|-----------|---------|-------------|
| `BOID_SIZE` | 1.5 | Visual size of boids |
| `MAX_SPEED` | 2.0 | Maximum velocity magnitude |
| `MIN_SPEED` | 0.5 | Minimum velocity magnitude |
| `MAX_FORCE` | 0.1 | Maximum steering force |

### Behavior Radii
| Parameter | Default | Description |
|-----------|---------|-------------|
| `SEPARATION_RADIUS` | 5.0 | Distance for separation behavior |
| `ALIGNMENT_RADIUS` | 15.0 | Distance for alignment behavior |
| `COHESION_RADIUS` | 20.0 | Distance for cohesion behavior |
| `OBSTACLE_RADIUS` | 15.0 | Detection distance for obstacles |
| `PREDATOR_RADIUS` | 25.0 | Detection distance for predator |

### Behavior Weights (runtime adjustable)
| Parameter | Default | Description |
|-----------|---------|-------------|
| `separationWeight` | 1.5 | Strength of separation |
| `alignmentWeight` | 1.0 | Strength of alignment |
| `cohesionWeight` | 1.0 | Strength of cohesion |
| `obstacleWeight` | 2.0 | Strength of obstacle avoidance |
| `predatorWeight` | 3.0 | Strength of predator evasion |
| `goalWeight` | 0.5 | Strength of goal seeking |
| `boundaryWeight` | 1.5 | Strength of boundary containment |

## Performance Notes

The system is optimized for smooth real-time simulation:

### Algorithm Complexity
- **Neighbor Search**: O(n²) - suitable for up to ~500 boids
- **Rendering**: O(n) draw calls per frame
- **Trail Updates**: O(n × trail_length)

### Optimizations
- **VAO/VBO Geometry**: Static geometry uploaded once
- **Dynamic Line Buffers**: Trails updated each frame
- **Shader-based Lighting**: GPU-accelerated Phong shading
- **Multisampling**: 4x MSAA for smooth edges

### Typical Performance
| Boid Count | Expected FPS |
|------------|--------------|
| 100 | 60+ FPS |
| 250 | 60 FPS |
| 500 | 30-60 FPS |
| 1000+ | < 30 FPS |


## Emergent Behaviors

When running the simulation, observe these phenomena:

1. **Flock Formation**: Boids naturally form cohesive groups
2. **Splitting and Merging**: Flocks split around obstacles and reform
3. **Predator Response**: Dramatic scattering when predator approaches
4. **Turning Dynamics**: Banking creates realistic turning visuals
5. **Edge Avoidance**: Smooth redirection at world boundaries
6. **Leader Following**: Emergent leaders based on position
