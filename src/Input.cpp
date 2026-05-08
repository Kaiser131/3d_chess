#include "Input.h"

void InputState::beginRightDrag(double x, double y) {
  rightMouseDown = true;
  lastX = x;
  lastY = y;
}

void InputState::endRightDrag() { rightMouseDown = false; }

void InputState::dragRight(Camera& cam, double x, double y) {
  if (!rightMouseDown) return;

  double dx = x - lastX;
  double dy = y - lastY;
  lastX = x;
  lastY = y;

  cam.orbit(static_cast<float>(dx) * 0.25f, static_cast<float>(-dy) * 0.25f);
}
