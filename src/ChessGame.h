#pragma once

#include "Board.h"
#include "Camera.h"
#include "Input.h"
#include "Renderer.h"

#include <GLFW/glfw3.h>

#include <optional>

struct ChessGame {
  GLFWwindow* window = nullptr;

  int fbWidth = 1280;
  int fbHeight = 720;

  Board board;
  Camera camera;
  InputState input;
  Renderer renderer;

  RenderSelection selection;

  bool init();
  void run();
  void shutdown();

  // callbacks
  void onFramebufferSize(int w, int h);
  void onMouseButton(int button, int action, int mods);
  void onCursorPos(double x, double y);
  void onScroll(double xoff, double yoff);
  void onKey(int key, int scancode, int action, int mods);

private:
  void clearSelection();
  void selectSquare(Square sq);
  void tryMoveTo(Square to);
};
