#pragma once

#include "Math.h"

struct Camera {
  // Orbit camera around target
  float yawDeg = -45.0f;
  float pitchDeg = 35.0f;
  float distance = 18.0f;
  Vec3 target{0.0f, 0.0f, 0.0f};

  void reset();
  void orbit(float deltaYawDeg, float deltaPitchDeg);
  void zoom(float delta);

  Vec3 position() const;
};
