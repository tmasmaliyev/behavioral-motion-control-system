#ifndef VECTOR3D_H
#define VECTOR3D_H

#include <cmath>
#include <glm/glm.hpp>

class Vector3D
{
public:
    float x, y, z;

    // Constructors
    Vector3D() : x(0), y(0), z(0) {}
    Vector3D(float x, float y, float z) : x(x), y(y), z(z) {}

    // Convert to glm::vec3 for OpenGL operations
    glm::vec3 toGLM() const { return glm::vec3(x, y, z); }

    // Vector addition
    Vector3D operator+(const Vector3D &v) const
    {
        return Vector3D(x + v.x, y + v.y, z + v.z);
    }

    // Vector subtraction
    Vector3D operator-(const Vector3D &v) const
    {
        return Vector3D(x - v.x, y - v.y, z - v.z);
    }

    // Scalar multiplication
    Vector3D operator*(float s) const
    {
        return Vector3D(x * s, y * s, z * s);
    }

    // Scalar division
    Vector3D operator/(float s) const
    {
        return s != 0 ? Vector3D(x / s, y / s, z / s) : Vector3D();
    }

    // Compound assignment operators
    Vector3D &operator+=(const Vector3D &v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    Vector3D &operator-=(const Vector3D &v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    Vector3D &operator*=(float s)
    {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    // Magnitude (length) of vector
    float magnitude() const
    {
        return sqrt(x * x + y * y + z * z);
    }

    // Squared magnitude (faster, avoids sqrt)
    float magnitudeSquared() const
    {
        return x * x + y * y + z * z;
    }

    // Normalize to unit vector
    Vector3D normalize() const
    {
        float mag = magnitude();
        return mag > 0 ? *this / mag : Vector3D();
    }

    // Limit magnitude to maximum value
    Vector3D limit(float maxMag) const
    {
        float mag = magnitude();
        return mag > maxMag ? normalize() * maxMag : *this;
    }

    // Dot product
    float dot(const Vector3D &v) const
    {
        return x * v.x + y * v.y + z * v.z;
    }

    // Cross product
    Vector3D cross(const Vector3D &v) const
    {
        return Vector3D(
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x);
    }

    // Distance to another vector
    float distanceTo(const Vector3D &v) const
    {
        return (*this - v).magnitude();
    }

    // Set to zero vector
    void zero()
    {
        x = y = z = 0;
    }

    // Check if zero vector
    bool isZero() const
    {
        return x == 0 && y == 0 && z == 0;
    }
};

#endif // VECTOR3D_H
