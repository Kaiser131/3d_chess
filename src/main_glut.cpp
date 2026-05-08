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

#include <memory>

static std::unique_ptr<GlutGame> g_game;

static void displayCb() {
  if (!g_game) return;
  g_game->updateAndRender();
  glutSwapBuffers();
}

static void reshapeCb(int w, int h) {
  if (!g_game) return;
  g_game->resize(w, h);
}

static void idleCb() {
  glutPostRedisplay();
}

static void mouseCb(int button, int state, int x, int y) {
  if (!g_game) return;
  g_game->onMouseButton(button, state, x, y);
}

static void motionCb(int x, int y) {
  if (!g_game) return;
  g_game->onMouseMove(x, y);
}

static void keyboardCb(unsigned char key, int x, int y) {
  if (!g_game) return;
  g_game->onKey(key, x, y);
}

static void specialCb(int key, int x, int y) {
  if (!g_game) return;
  g_game->onSpecialKey(key, x, y);
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
  glutInitWindowSize(1280, 720);
  glutCreateWindow("3D Chess (OpenGL + freeglut)");

  g_game.reset(new GlutGame());
  g_game->initGL();

  glutDisplayFunc(displayCb);
  glutReshapeFunc(reshapeCb);
  glutIdleFunc(idleCb);
  glutMouseFunc(mouseCb);
  glutMotionFunc(motionCb);
  glutKeyboardFunc(keyboardCb);
  glutSpecialFunc(specialCb);

  glutMainLoop();
  return 0;
}
