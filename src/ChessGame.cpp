#include "ChessGame.h"

#include <cstdio>

static void fbSizeCb(GLFWwindow* w, int width, int height) {
  auto* g = static_cast<ChessGame*>(glfwGetWindowUserPointer(w));
  if (g) g->onFramebufferSize(width, height);
}

static void mouseBtnCb(GLFWwindow* w, int button, int action, int mods) {
  auto* g = static_cast<ChessGame*>(glfwGetWindowUserPointer(w));
  if (g) g->onMouseButton(button, action, mods);
}

static void cursorPosCb(GLFWwindow* w, double x, double y) {
  auto* g = static_cast<ChessGame*>(glfwGetWindowUserPointer(w));
  if (g) g->onCursorPos(x, y);
}

static void scrollCb(GLFWwindow* w, double xoff, double yoff) {
  auto* g = static_cast<ChessGame*>(glfwGetWindowUserPointer(w));
  if (g) g->onScroll(xoff, yoff);
}

static void keyCb(GLFWwindow* w, int key, int scancode, int action, int mods) {
  auto* g = static_cast<ChessGame*>(glfwGetWindowUserPointer(w));
  if (g) g->onKey(key, scancode, action, mods);
}

bool ChessGame::init() {
  camera.reset();
  board.reset();
  clearSelection();

  if (!glfwInit()) {
    std::fprintf(stderr, "GLFW init failed\n");
    return false;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_SAMPLES, 4);

  window = glfwCreateWindow(1280, 720, "3D Chess (OpenGL + GLFW)", nullptr, nullptr);
  if (!window) {
    std::fprintf(stderr, "Failed to create window\n");
    glfwTerminate();
    return false;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  glfwSetWindowUserPointer(window, this);
  glfwSetFramebufferSizeCallback(window, fbSizeCb);
  glfwSetMouseButtonCallback(window, mouseBtnCb);
  glfwSetCursorPosCallback(window, cursorPosCb);
  glfwSetScrollCallback(window, scrollCb);
  glfwSetKeyCallback(window, keyCb);

  glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
  renderer.initGL();
  renderer.resize(fbWidth, fbHeight);

  return true;
}

void ChessGame::shutdown() {
  if (window) {
    glfwDestroyWindow(window);
    window = nullptr;
  }
  glfwTerminate();
}

void ChessGame::run() {
  while (window && !glfwWindowShouldClose(window)) {
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

    renderer.beginFrame(camera, fbWidth, fbHeight);
    renderer.drawScene(board, selection);
    renderer.endFrame();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}

void ChessGame::onFramebufferSize(int w, int h) {
  fbWidth = w;
  fbHeight = h;
  renderer.resize(w, h);
}

void ChessGame::clearSelection() {
  selection.hasSelected = false;
  selection.validMoves.clear();
}

void ChessGame::selectSquare(Square sq) {
  selection.hasSelected = true;
  selection.selected = sq;
  selection.validMoves = board.legalMovesFrom(sq);
}

void ChessGame::tryMoveTo(Square to) {
  if (!selection.hasSelected) return;
  Square from = selection.selected;

  if (board.tryMakeMove(from, to)) {
    clearSelection();
  } else {
    // If user clicks another own piece, switch selection
    const Piece* p = board.at(to);
    if (p && p->color == board.sideToMove) {
      selectSquare(to);
    }
  }
}

void ChessGame::onMouseButton(int button, int action, int /*mods*/) {
  if (button == GLFW_MOUSE_BUTTON_RIGHT) {
    if (action == GLFW_PRESS) {
      double x, y;
      glfwGetCursorPos(window, &x, &y);
      input.beginRightDrag(x, y);
    } else if (action == GLFW_RELEASE) {
      input.endRightDrag();
    }
    return;
  }

  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    double x, y;
    glfwGetCursorPos(window, &x, &y);

    // Cursor position is in window coordinates; convert to framebuffer coordinates for picking
    int winW = 1, winH = 1;
    glfwGetWindowSize(window, &winW, &winH);
    if (winW > 0 && winH > 0) {
      x *= static_cast<double>(fbWidth) / static_cast<double>(winW);
      y *= static_cast<double>(fbHeight) / static_cast<double>(winH);
    }

    // Need current matrices (renderer caches them in beginFrame). If the click happens
    // between frames, the matrices are still valid for this frame's camera.
    Square hit;
    if (!renderer.screenToBoardSquare(x, y, fbWidth, fbHeight, hit)) return;

    if (!selection.selected.has_value()) {
      const Piece* p = board.at(hit);
      if (p && p->color == board.sideToMove) {
        selectSquare(hit);
      }
      return;
    }

    // Second click: try move
    if (selection.hasSelected && selection.selected == hit) {
      clearSelection();
      return;
    }

    tryMoveTo(hit);
  }
}

void ChessGame::onCursorPos(double x, double y) {
  input.dragRight(camera, x, y);
}

void ChessGame::onScroll(double /*xoff*/, double yoff) {
  camera.zoom(static_cast<float>(-yoff) * 0.8f);
}

void ChessGame::onKey(int key, int /*scancode*/, int action, int /*mods*/) {
  if (action != GLFW_PRESS) return;

  if (key == GLFW_KEY_ESCAPE) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  } else if (key == GLFW_KEY_R) {
    camera.reset();
  }
}
