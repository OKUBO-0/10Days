#ifndef VECTOR3_H
#define VECTOR3_H
#include "Matrix4x4.h"  // Matrix4x4 の定義をインクルード

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
    Vector3 operator-(const Vector3& other) const {
        return Vector3(this->x - other.x, this->y - other.y, this->z - other.z);
    }

    Vector3& operator-=(const Vector3& other) {
        this->x -= other.x;
        this->y -= other.y;
        this->z -= other.z;
        return *this;
    }
};

// MultiplyMatrixVector を Vector3 クラス外に静的関数として定義する
inline Vector3 MultiplyMatrixVector(const Matrix4x4& mat, const Vector3& vec) {
    Vector3 result;
    result.x = mat.m[0][0] * vec.x + mat.m[1][0] * vec.y + mat.m[2][0] * vec.z + mat.m[3][0];
    result.y = mat.m[0][1] * vec.x + mat.m[1][1] * vec.y + mat.m[2][1] * vec.z + mat.m[3][1];
    result.z = mat.m[0][2] * vec.x + mat.m[1][2] * vec.y + mat.m[2][2] * vec.z + mat.m[3][2];
    return result;
}

#endif // VECTOR3_H