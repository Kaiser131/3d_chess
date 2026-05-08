#pragma once

#include <cmath>

struct Vec3 {
  float x;
  float y;
  float z;

  Vec3() : x(0.0f), y(0.0f), z(0.0f) {}
  Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
};

inline Vec3 operator+(const Vec3& a, const Vec3& b) { return Vec3(a.x + b.x, a.y + b.y, a.z + b.z); }
inline Vec3 operator-(const Vec3& a, const Vec3& b) { return Vec3(a.x - b.x, a.y - b.y, a.z - b.z); }
inline Vec3 operator*(const Vec3& a, float s) { return Vec3(a.x * s, a.y * s, a.z * s); }

inline float dot(const Vec3& a, const Vec3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

inline float length(const Vec3& v) { return std::sqrt(dot(v, v)); }

inline Vec3 normalize(const Vec3& v) {
  float len = length(v);
  if (len <= 1e-6f) return Vec3(0.0f, 0.0f, 0.0f);
  return Vec3(v.x / len, v.y / len, v.z / len);
}
