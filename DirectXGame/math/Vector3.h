#ifndef VECTOR3_H
#define VECTOR3_H

class Vector3 {
public:
    float x, y, z;

    // Constructor
    Vector3(float x = 0.0f, float y = 0.0f, float z = 0.0f)
        : x(x), y(y), z(z) {}

    // += operator overload
    Vector3& operator+=(const Vector3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;  // Return reference to self for chaining
    }

    // Example of + operator overload (as a non-member function)
    friend const Vector3 operator+(const Vector3& v1, const Vector3& v2);

    // Other methods and operators can be defined here
};

#endif // VECTOR3_H