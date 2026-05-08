#include "GlutGame.h"

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cstdio>

void GlutGame::initGL() {
  camera.reset();
  board.reset();
  clearSelection();
  renderer.initGL();
  renderer.resize(width, height);
}

void GlutGame::resize(int w, int h) {
  width = (w > 1) ? w : 1;
  height = (h > 1) ? h : 1;
  renderer.resize(width, height);
}

void GlutGame::updateAndRender() {
  renderer.beginFrame(camera, width, height);
  renderer.drawScene(board, selection);
  renderer.endFrame();
}

void GlutGame::clearSelection() {
  selection.hasSelected = false;
  selection.validMoves.clear();
}

void GlutGame::selectSquare(Square sq) {
  selection.hasSelected = true;
  selection.selected = sq;
  selection.validMoves = board.legalMovesFrom(sq);
}

void GlutGame::tryMoveTo(Square to) {
  if (!selection.hasSelected) return;
  Square from = selection.selected;

  if (board.tryMakeMove(from, to)) {
    clearSelection();
  } else {
    const Piece* p = board.at(to);
    if (p && p->color == board.sideToMove) {
      selectSquare(to);
    }
  }
}

void GlutGame::onMouseButton(int button, int state, int x, int y) {
  // Mouse wheel (common freeglut mapping)
  if (state == GLUT_DOWN) {
    if (button == 3) camera.zoom(-0.8f);
    if (button == 4) camera.zoom(0.8f);
  }

  if (button == GLUT_RIGHT_BUTTON || button == GLUT_MIDDLE_BUTTON) {
    if (state == GLUT_DOWN) input.beginRightDrag(x, y);
    if (state == GLUT_UP) input.endRightDrag();
    return;
  }

  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    Square hit;
    if (!renderer.screenToBoardSquare(static_cast<double>(x), static_cast<double>(y), width, height, hit)) return;

    if (!selection.hasSelected) {
      const Piece* p = board.at(hit);
      if (p && p->color == board.sideToMove) selectSquare(hit);
      return;
    }

    if (selection.selected == hit) {
      clearSelection();
      return;
    }

    tryMoveTo(hit);
  }
}

void GlutGame::onMouseMove(int x, int y) {
  input.dragRight(camera, x, y);
}

void GlutGame::onKey(unsigned char key, int /*x*/, int /*y*/) {
  if (key == 27) { // ESC
    // freeglut supports leaving the main loop for a clean shutdown
    #ifdef FREEGLUT
    glutLeaveMainLoop();
    #else
    std::exit(0);
    #endif
  } else if (key == 'r' || key == 'R') {
    camera.reset();
  } else if (key == '+' || key == '=') {
    camera.zoom(-0.8f);
  } else if (key == '-' || key == '_') {
    camera.zoom(0.8f);
  }
}

void GlutGame::onSpecialKey(int key, int /*x*/, int /*y*/) {
  // Arrow keys orbit camera for reliable demos
  const float step = 4.0f;
  if (key == GLUT_KEY_LEFT) camera.orbit(-step, 0.0f);
  if (key == GLUT_KEY_RIGHT) camera.orbit(step, 0.0f);
  if (key == GLUT_KEY_UP) camera.orbit(0.0f, step);
  if (key == GLUT_KEY_DOWN) camera.orbit(0.0f, -step);
}
