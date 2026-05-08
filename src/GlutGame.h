#pragma once

#include "Board.h"
#include "Camera.h"
#include "Input.h"
#include "Renderer.h"

struct GlutGame {
  int width = 1280;
  int height = 720;

  Board board;
  Camera camera;
  InputState input;
  Renderer renderer;

  RenderSelection selection;

  void initGL();
  void resize(int w, int h);
  void updateAndRender();

  void onMouseButton(int button, int state, int x, int y);
  void onMouseMove(int x, int y);
  void onKey(unsigned char key, int x, int y);
  void onSpecialKey(int key, int x, int y);

private:
  void clearSelection();
  void selectSquare(Square sq);
  void tryMoveTo(Square to);
};
