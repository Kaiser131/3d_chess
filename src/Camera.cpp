#include "Camera.h"

#include <algorithm>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static float degToRad(float d) { return d * static_cast<float>(M_PI) / 180.0f; }

void Camera::reset() {
  yawDeg = -45.0f;
  pitchDeg = 35.0f;
  distance = 18.0f;
  target = {0.0f, 0.0f, 0.0f};
}

void Camera::orbit(float deltaYawDeg, float deltaPitchDeg) {
  yawDeg += deltaYawDeg;
  pitchDeg = std::clamp(pitchDeg + deltaPitchDeg, -10.0f, 85.0f);
}

void Camera::zoom(float delta) {
  distance = std::clamp(distance + delta, 7.0f, 40.0f);
}

Vec3 Camera::position() const {
  float yaw = degToRad(yawDeg);
  float pitch = degToRad(pitchDeg);

  float cp = std::cos(pitch);
  float sp = std::sin(pitch);
  float cy = std::cos(yaw);
  float sy = std::sin(yaw);

  Vec3 offset{
      distance * cp * cy,
      distance * sp,
      distance * cp * sy,
  };
  return target + offset;
}
