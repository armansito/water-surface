#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>
#include <stdlib.h>
#include <iostream>

inline int min(int a, int b) { return (a < b) ? a : b; }
inline int max(int a, int b) { return (a > b) ? a : b; }

// Returns a random number between 0 and 1
inline double frand() { return (double)rand() / (double)RAND_MAX; }
inline float frandf() { return (float)rand() / (float)RAND_MAX; }

class Vector2
{
public:
    // This union provides both individual members "x" and "y" and an array "xy"
    // that are automatically in sync (since they actually refer to the same data).
    // This is useful for OpenGL to pass vectors to the gl*2fv() functions:
    //
    //     Vector2 vec(1, 2);
    //     glVertex2fv(vec.xy);
    //
    union
    {
        struct { float x, y; };
        float xy[2];
    };

    Vector2() : x(0), y(0) {}
    Vector2(float x, float y) : x(x), y(y) {}
    Vector2(const Vector2 &vec) : x(vec.x), y(vec.y) {}

    Vector2 operator + () const { return Vector2(+x, +y); }
    Vector2 operator - () const { return Vector2(-x, -y); }

    Vector2 operator + (const Vector2 &vec) const { return Vector2(x + vec.x, y + vec.y); }
    Vector2 operator - (const Vector2 &vec) const { return Vector2(x - vec.x, y - vec.y); }
    Vector2 operator * (const Vector2 &vec) const { return Vector2(x * vec.x, y * vec.y); }
    Vector2 operator / (const Vector2 &vec) const { return Vector2(x / vec.x, y / vec.y); }
    Vector2 operator + (float s) const { return Vector2(x + s, y + s); }
    Vector2 operator - (float s) const { return Vector2(x - s, y - s); }
    Vector2 operator * (float s) const { return Vector2(x * s, y * s); }
    Vector2 operator / (float s) const { return Vector2(x / s, y / s); }

    friend Vector2 operator + (float s, const Vector2 &vec) { return Vector2(s + vec.x, s + vec.y); }
    friend Vector2 operator - (float s, const Vector2 &vec) { return Vector2(s - vec.x, s - vec.y); }
    friend Vector2 operator * (float s, const Vector2 &vec) { return Vector2(s * vec.x, s * vec.y); }
    friend Vector2 operator / (float s, const Vector2 &vec) { return Vector2(s / vec.x, s / vec.y); }

    Vector2 &operator += (const Vector2 &vec) { return *this = *this + vec; }
    Vector2 &operator -= (const Vector2 &vec) { return *this = *this - vec; }
    Vector2 &operator *= (const Vector2 &vec) { return *this = *this * vec; }
    Vector2 &operator /= (const Vector2 &vec) { return *this = *this / vec; }
    Vector2 &operator += (float s) { return *this = *this + s; }
    Vector2 &operator -= (float s) { return *this = *this - s; }
    Vector2 &operator *= (float s) { return *this = *this * s; }
    Vector2 &operator /= (float s) { return *this = *this / s; }

    bool operator == (const Vector2 &vec) const { return x == vec.x && y == vec.y; }
    bool operator != (const Vector2 &vec) const { return x != vec.x || y != vec.y; }

    float lengthSquared() const { return x * x + y * y; }
    float length() const { return sqrtf(lengthSquared()); }
    float dot(const Vector2 &vec) const { return x * vec.x + y * vec.y; }
    Vector2 unit() const { return *this / length(); }
    void normalize() { *this = unit(); }

    // Fraction should be a value in the range [0, 1]
    static Vector2 lerp(const Vector2 &a, const Vector2 &b, float fraction) { return a + (b - a) * fraction; }
    static Vector2 lerp(const Vector2 &a, const Vector2 &b, const Vector2 &fraction) { return a + (b - a) * fraction; }

    float min() const { return fminf(x, y); }
    float max() const { return fmaxf(x, y); }
    static Vector2 min(const Vector2 &a, const Vector2 &b) { return Vector2(fminf(a.x, b.x), fminf(a.y, b.y)); }
    static Vector2 max(const Vector2 &a, const Vector2 &b) { return Vector2(fmaxf(a.x, b.x), fmaxf(a.y, b.y)); }

    Vector2 floor() const { return Vector2(floorf(x), floorf(y)); }
    Vector2 ceil() const { return Vector2(ceilf(x), ceilf(y)); }
    Vector2 abs() const { return Vector2(fabsf(x), fabsf(y)); }

    // Returns an angle in the range [-pi, pi]
    float toAngle() const { return atan2f(y, x); }
    static Vector2 fromAngle(float theta) { return Vector2(cosf(theta), sinf(theta)); }

    // Returns a uniformly-random unit vector
    static Vector2 randomDirection() { return fromAngle(frand() * 2 * M_PI); }
};

class Vector3
{
public:
    // This union provides both individual members "x", "y", and "z" an array "xyz"
    // that are automatically in sync (since they actually refer to the same data).
    // This is useful for OpenGL to pass vectors to the gl*3fv() functions:
    //
    //     Vector3 vec(1, 2, 3);
    //     glVertex3fv(vec.xyz);
    //
    union
    {
        struct { float x, y, z; };
        float xyz[3];
    };

    Vector3() : x(0), y(0), z(0) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
    Vector3(const Vector3 &vec) : x(vec.x), y(vec.y), z(vec.z) {}

    Vector3 operator + () const { return Vector3(+x, +y, +z); }
    Vector3 operator - () const { return Vector3(-x, -y, -z); }

    Vector3 operator + (const Vector3 &vec) const { return Vector3(x + vec.x, y + vec.y, z + vec.z); }
    Vector3 operator - (const Vector3 &vec) const { return Vector3(x - vec.x, y - vec.y, z - vec.z); }
    Vector3 operator * (const Vector3 &vec) const { return Vector3(x * vec.x, y * vec.y, z * vec.z); }
    Vector3 operator / (const Vector3 &vec) const { return Vector3(x / vec.x, y / vec.y, z / vec.z); }
    Vector3 operator + (float s) const { return Vector3(x + s, y + s, z + s); }
    Vector3 operator - (float s) const { return Vector3(x - s, y - s, z - s); }
    Vector3 operator * (float s) const { return Vector3(x * s, y * s, z * s); }
    Vector3 operator / (float s) const { return Vector3(x / s, y / s, z / s); }

    friend Vector3 operator + (float s, const Vector3 &vec) { return Vector3(s + vec.x, s + vec.y, s + vec.z); }
    friend Vector3 operator - (float s, const Vector3 &vec) { return Vector3(s - vec.x, s - vec.y, s - vec.z); }
    friend Vector3 operator * (float s, const Vector3 &vec) { return Vector3(s * vec.x, s * vec.y, s * vec.z); }
    friend Vector3 operator / (float s, const Vector3 &vec) { return Vector3(s / vec.x, s / vec.y, s / vec.z); }

    Vector3 &operator += (const Vector3 &vec) { return *this = *this + vec; }
    Vector3 &operator -= (const Vector3 &vec) { return *this = *this - vec; }
    Vector3 &operator *= (const Vector3 &vec) { return *this = *this * vec; }
    Vector3 &operator /= (const Vector3 &vec) { return *this = *this / vec; }
    Vector3 &operator += (float s) { return *this = *this + s; }
    Vector3 &operator -= (float s) { return *this = *this - s; }
    Vector3 &operator *= (float s) { return *this = *this * s; }
    Vector3 &operator /= (float s) { return *this = *this / s; }

    bool operator == (const Vector3 &vec) const { return x == vec.x && y == vec.y && z == vec.z; }
    bool operator != (const Vector3 &vec) const { return x != vec.x || y != vec.y || z != vec.z; }

    float lengthSquared() const { return x * x + y * y + z * z; }
    float length() const { return sqrtf(lengthSquared()); }
    float dot(const Vector3 &vec) const { return x * vec.x + y * vec.y + z * vec.z; }
    Vector3 cross(const Vector3 &vec) const { return Vector3(y * vec.z - z * vec.y, z * vec.x - x * vec.z, x * vec.y - y * vec.x); }
    Vector3 unit() const { return *this / length(); }
    void normalize() { *this = unit(); }

    // Fraction should be a value in the range [0, 1]
    static Vector3 lerp(const Vector3 &a, const Vector3 &b, float fraction) { return a + (b - a) * fraction; }
    static Vector3 lerp(const Vector3 &a, const Vector3 &b, const Vector3 &fraction) { return a + (b - a) * fraction; }

    float min() const { return fminf(x, fminf(y, z)); }
    float max() const { return fmaxf(x, fmaxf(y, z)); }
    static Vector3 min(const Vector3 &a, const Vector3 &b) { return Vector3(fminf(a.x, b.x), fminf(a.y, b.y), fminf(a.z, b.z)); }
    static Vector3 max(const Vector3 &a, const Vector3 &b) { return Vector3(fmaxf(a.x, b.x), fmaxf(a.y, b.y), fmaxf(a.z, b.z)); }

    Vector3 floor() const { return Vector3(floorf(x), floorf(y), floorf(z)); }
    Vector3 ceil() const { return Vector3(ceilf(x), ceilf(y), ceilf(z)); }
    Vector3 abs() const { return Vector3(fabsf(x), fabsf(y), fabsf(z)); }

    // Returns two angles in the range [-pi, pi] and [-pi/2, pi/2], respectively
    Vector2 toAngles() const { return Vector2(atan2f(z, x), asinf(y / length())); }

    // Returns a unit vector from the spherical coordinates (1, phi, theta) (assumes +y is up)
    static Vector3 fromAngles(float theta, float phi) { return Vector3(cosf(theta) * cosf(phi), sinf(phi), sinf(theta) * cosf(phi)); }
    static Vector3 fromAngles(const Vector2 &angles) { return fromAngles(angles.x, angles.y); }

    // Returns a uniformly-random unit vector
    static Vector3 randomDirection() { return fromAngles(frand() * 2 * M_PI, asinf(frand() * 2 - 1)); }
};

class Vector4
{
public:
    // This union provides both individual members "x", "y", "z", and "w" an array "xyzw"
    // that are automatically in sync (since they actually refer to the same data).
    // This is useful for OpenGL to pass vectors to the gl*4fv() functions:
    //
    //     Vector4 vec(1, 2, 3, 1);
    //     glVertex4fv(vec.xyz);
    //
    union
    {
        struct { float x, y, z, w; };
        float xyzw[4];
    };

    Vector4() : x(0), y(0), z(0), w(0) {}
    Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    Vector4(const Vector4 &vec) : x(vec.x), y(vec.y), z(vec.z), w(vec.w) {}
    Vector4(const Vector3 &vec, float w) : x(vec.x), y(vec.y), z(vec.z), w(w) {}

    Vector4 operator + () const { return Vector4(+x, +y, +z, +w); }
    Vector4 operator - () const { return Vector4(-x, -y, -z, -w); }

    Vector4 operator + (const Vector4 &vec) const { return Vector4(x + vec.x, y + vec.y, z + vec.z, w + vec.w); }
    Vector4 operator - (const Vector4 &vec) const { return Vector4(x - vec.x, y - vec.y, z - vec.z, w - vec.w); }
    Vector4 operator * (const Vector4 &vec) const { return Vector4(x * vec.x, y * vec.y, z * vec.z, w * vec.w); }
    Vector4 operator / (const Vector4 &vec) const { return Vector4(x / vec.x, y / vec.y, z / vec.z, w / vec.w); }
    Vector4 operator + (float s) const { return Vector4(x + s, y + s, z + s, w + s); }
    Vector4 operator - (float s) const { return Vector4(x - s, y - s, z - s, w - s); }
    Vector4 operator * (float s) const { return Vector4(x * s, y * s, z * s, w * s); }
    Vector4 operator / (float s) const { return Vector4(x / s, y / s, z / s, w / s); }

    friend Vector4 operator + (float s, const Vector4 &vec) { return Vector4(s + vec.x, s + vec.y, s + vec.z, s + vec.w); }
    friend Vector4 operator - (float s, const Vector4 &vec) { return Vector4(s - vec.x, s - vec.y, s - vec.z, s - vec.w); }
    friend Vector4 operator * (float s, const Vector4 &vec) { return Vector4(s * vec.x, s * vec.y, s * vec.z, s * vec.w); }
    friend Vector4 operator / (float s, const Vector4 &vec) { return Vector4(s / vec.x, s / vec.y, s / vec.z, s / vec.w); }

    Vector4 &operator += (const Vector4 &vec) { return *this = *this + vec; }
    Vector4 &operator -= (const Vector4 &vec) { return *this = *this - vec; }
    Vector4 &operator *= (const Vector4 &vec) { return *this = *this * vec; }
    Vector4 &operator /= (const Vector4 &vec) { return *this = *this / vec; }
    Vector4 &operator += (float s) { return *this = *this + s; }
    Vector4 &operator -= (float s) { return *this = *this - s; }
    Vector4 &operator *= (float s) { return *this = *this * s; }
    Vector4 &operator /= (float s) { return *this = *this / s; }

    bool operator == (const Vector4 &vec) const { return x == vec.x && y == vec.y && z == vec.z && w == vec.w; }
    bool operator != (const Vector4 &vec) const { return x != vec.x || y != vec.y || z != vec.z || w != vec.w; }

    float lengthSquared() const { return x * x + y * y + z * z + w * w; }
    float length() const { return sqrtf(lengthSquared()); }
    float dot(const Vector4 &vec) const { return x * vec.x + y * vec.y + z * vec.z + w * vec.w; }
    Vector4 homogenized() const { return *this / w; }
    void homogenize() { *this /= w; }

    // Fraction should be a value in the range [0, 1]
    static Vector4 lerp(const Vector4 &a, const Vector4 &b, float fraction) { return a + (b - a) * fraction; }
    static Vector4 lerp(const Vector4 &a, const Vector4 &b, const Vector4 &fraction) { return a + (b - a) * fraction; }

    float min() const { return fminf(fminf(x, y), fminf(z, w)); }
    float max() const { return fmaxf(fmaxf(x, y), fmaxf(z, w)); }
    static Vector4 min(const Vector4 &a, const Vector4 &b) { return Vector4(fminf(a.x, b.x), fminf(a.y, b.y), fminf(a.z, b.z), fminf(a.w, b.w)); }
    static Vector4 max(const Vector4 &a, const Vector4 &b) { return Vector4(fmaxf(a.x, b.x), fmaxf(a.y, b.y), fmaxf(a.z, b.z), fmaxf(a.w, b.w)); }

    Vector4 floor() const { return Vector4(floorf(x), floorf(y), floorf(z), floorf(w)); }
    Vector4 ceil() const { return Vector4(ceilf(x), ceilf(y), ceilf(z), ceilf(w)); }
    Vector4 abs() const { return Vector4(fabsf(x), fabsf(y), fabsf(z), fabsf(w)); }
};

inline std::ostream &operator << (std::ostream &out, const Vector2 &v) { return out << "(" << v.x << ", " << v.y << ")"; }
inline std::ostream &operator << (std::ostream &out, const Vector3 &v) { return out << "(" << v.x << ", " << v.y << ", " << v.z << ")"; }
inline std::ostream &operator << (std::ostream &out, const Vector4 &v) { return out << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")"; }

#endif // VECTOR_H

