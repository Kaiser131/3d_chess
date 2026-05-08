#pragma once

#include "Camera.h"

struct InputState {
  bool rightMouseDown = false;
  double lastX = 0.0;
  double lastY = 0.0;

  void beginRightDrag(double x, double y);
  void endRightDrag();
  void dragRight(Camera& cam, double x, double y);
};
