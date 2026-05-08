#include "Renderer.h"

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#include <algorithm>
#include <cmath>

static Vec3 squareCenter(int file, int rank) {
  // file 0..7 => x from -3.5..+3.5
  // rank 0..7 => z from +3.5..-3.5 (white home rank at +z)
  return {
      -3.5f + static_cast<float>(file),
      0.0f,
      3.5f - static_cast<float>(rank),
  };
}

void Renderer::initGL() {
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_NORMALIZE);

  glShadeModel(GL_SMOOTH);
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

  #ifdef GL_MULTISAMPLE
  glEnable(GL_MULTISAMPLE);
  #endif

  GLfloat ambient[] = {0.25f, 0.25f, 0.25f, 1.0f};
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

  GLfloat lightPos[] = {12.0f, 18.0f, 8.0f, 1.0f};
  GLfloat diffuse[] = {0.95f, 0.95f, 0.95f, 1.0f};
  GLfloat spec[] = {0.6f, 0.6f, 0.6f, 1.0f};
  glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
}

void Renderer::resize(int width, int height) {
  glViewport(0, 0, width, height);
}

void Renderer::beginFrame(const Camera& camera, int width, int height) {
  glViewport(0, 0, width, height);

  glClearColor(0.08f, 0.09f, 0.10f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  float aspect = (height > 0) ? (static_cast<float>(width) / static_cast<float>(height)) : 1.0f;
  gluPerspective(50.0, aspect, 0.2, 200.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  Vec3 eye = camera.position();
  Vec3 at = camera.target;
  gluLookAt(eye.x, eye.y, eye.z, at.x, at.y, at.z, 0.0, 1.0, 0.0);

  // Update light position after view transform so it stays world-relative
  GLfloat lightPos[] = {12.0f, 18.0f, 8.0f, 1.0f};
  glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

  cacheMatrices();
}

void Renderer::endFrame() {
  // no-op; swap handled by caller
}

void Renderer::cacheMatrices() const {
  glGetIntegerv(GL_VIEWPORT, viewport_);
  glGetDoublev(GL_MODELVIEW_MATRIX, modelview_);
  glGetDoublev(GL_PROJECTION_MATRIX, projection_);
}

void Renderer::drawScene(const Board& board, const RenderSelection& selection) {
  drawBoard(selection);
  drawPieces(board, selection);
}

void Renderer::setMaterialForTile(bool dark, bool highlight) const {
  float base = dark ? 0.20f : 0.80f;
  float r = base;
  float g = base;
  float b = base;

  if (highlight) {
    // soft yellow highlight
    r = std::min(1.0f, r + 0.25f);
    g = std::min(1.0f, g + 0.25f);
  }

  glColor3f(r, g, b);

  GLfloat spec[] = {0.08f, 0.08f, 0.08f, 1.0f};
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 8.0f);
}

void Renderer::setMaterialForPiece(PieceColor color, bool selected) const {
  float r, g, b;
  if (color == PieceColor::White) {
    r = 0.93f; g = 0.92f; b = 0.86f;
  } else {
    r = 0.20f; g = 0.22f; b = 0.25f;
  }

  if (selected) {
    // tint for selection
    r = std::min(1.0f, r + 0.20f);
    g = std::min(1.0f, g + 0.20f);
  }

  glColor3f(r, g, b);

  GLfloat spec[] = {0.45f, 0.45f, 0.45f, 1.0f};
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 32.0f);
}

void Renderer::drawTile(int file, int rank, bool dark, bool highlight) const {
  Vec3 c = squareCenter(file, rank);
  float y = 0.0f;

  setMaterialForTile(dark, highlight);

  glBegin(GL_QUADS);
  glNormal3f(0.0f, 1.0f, 0.0f);
  // CCW winding when viewed from +Y
  glVertex3f(c.x - 0.5f, y, c.z - 0.5f);
  glVertex3f(c.x - 0.5f, y, c.z + 0.5f);
  glVertex3f(c.x + 0.5f, y, c.z + 0.5f);
  glVertex3f(c.x + 0.5f, y, c.z - 0.5f);
  glEnd();
}

void Renderer::drawHighlightMarker(int file, int rank, bool isCapture) const {
  Vec3 c = squareCenter(file, rank);
  float y = 0.001f;

  glDisable(GL_LIGHTING);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  if (isCapture) {
    glColor4f(0.90f, 0.20f, 0.20f, 0.45f);
  } else {
    glColor4f(0.20f, 0.80f, 0.35f, 0.35f);
  }

  // draw a slightly inset quad
  glBegin(GL_QUADS);
  // CCW winding when viewed from +Y
  glVertex3f(c.x - 0.35f, y, c.z - 0.35f);
  glVertex3f(c.x - 0.35f, y, c.z + 0.35f);
  glVertex3f(c.x + 0.35f, y, c.z + 0.35f);
  glVertex3f(c.x + 0.35f, y, c.z - 0.35f);
  glEnd();

  glDisable(GL_BLEND);
  glEnable(GL_LIGHTING);
}

void Renderer::drawBoard(const RenderSelection& selection) {
  // Board base block (gives thickness)
  {
    glPushMatrix();
    // Centered at origin; spans 8x8, with a small margin
    float half = 4.2f;
    float topY = -0.06f;
    float bottomY = -0.45f;

    glDisable(GL_COLOR_MATERIAL);
    GLfloat diffuse[] = {0.16f, 0.12f, 0.10f, 1.0f};
    GLfloat spec[] = {0.10f, 0.10f, 0.10f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 12.0f);

    glBegin(GL_QUADS);
    // top
    glNormal3f(0, 1, 0);
    // CCW winding when viewed from +Y
    glVertex3f(-half, topY, -half);
    glVertex3f(-half, topY, half);
    glVertex3f(half, topY, half);
    glVertex3f(half, topY, -half);
    // bottom
    glNormal3f(0, -1, 0);
    glVertex3f(-half, bottomY, -half);
    glVertex3f(-half, bottomY, half);
    glVertex3f(half, bottomY, half);
    glVertex3f(half, bottomY, -half);
    // +Z
    glNormal3f(0, 0, 1);
    glVertex3f(-half, bottomY, half);
    glVertex3f(-half, topY, half);
    glVertex3f(half, topY, half);
    glVertex3f(half, bottomY, half);
    // -Z
    glNormal3f(0, 0, -1);
    glVertex3f(-half, bottomY, -half);
    glVertex3f(half, bottomY, -half);
    glVertex3f(half, topY, -half);
    glVertex3f(-half, topY, -half);
    // +X
    glNormal3f(1, 0, 0);
    glVertex3f(half, bottomY, -half);
    glVertex3f(half, bottomY, half);
    glVertex3f(half, topY, half);
    glVertex3f(half, topY, -half);
    // -X
    glNormal3f(-1, 0, 0);
    glVertex3f(-half, bottomY, -half);
    glVertex3f(-half, topY, -half);
    glVertex3f(-half, topY, half);
    glVertex3f(-half, bottomY, half);
    glEnd();

    glEnable(GL_COLOR_MATERIAL);
    glPopMatrix();
  }

  for (int f = 0; f < 8; ++f) {
    for (int r = 0; r < 8; ++r) {
      bool dark = ((f + r) % 2) == 1;
      bool highlight = selection.hasSelected && selection.selected.file == f && selection.selected.rank == r;
      drawTile(f, r, dark, highlight);
    }
  }
}

void Renderer::drawPieces(const Board& board, const RenderSelection& selection) {
  // Valid move markers depend on occupancy (capture vs quiet)
  for (const Square& s : selection.validMoves) {
    bool isCapture = (board.at(s) != nullptr);
    drawHighlightMarker(s.file, s.rank, isCapture);
  }

  for (int f = 0; f < 8; ++f) {
    for (int r = 0; r < 8; ++r) {
      Square sq{f, r};
      const Piece* p = board.at(sq);
      if (!p) continue;

      bool isSelected = selection.hasSelected && (selection.selected == sq);
      setMaterialForPiece(p->color, isSelected);

      Vec3 c = squareCenter(f, r);
      glPushMatrix();
      glTranslatef(c.x, 0.02f, c.z);

      // Orient black pieces to face opposite direction (mainly relevant for the knight)
      if (p->color == PieceColor::Black) {
        glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
      }

      // Scale down to fit nicely
      glScalef(0.9f, 0.9f, 0.9f);

      drawPieceModel(p->type);

      glPopMatrix();
    }
  }
}

void Renderer::drawPieceModel(PieceType type) const {
  switch (type) {
    case PieceType::Pawn: drawPawn(); break;
    case PieceType::Rook: drawRook(); break;
    case PieceType::Knight: drawKnight(); break;
    case PieceType::Bishop: drawBishop(); break;
    case PieceType::Queen: drawQueen(); break;
    case PieceType::King: drawKing(); break;
  }
}

static void solidCylinder(float r0, float r1, float h, int slices) {
  GLUquadric* q = gluNewQuadric();
  gluQuadricNormals(q, GLU_SMOOTH);
  gluCylinder(q, r0, r1, h, slices, 1);

  // caps
  glPushMatrix();
  glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
  gluDisk(q, 0.0, r0, slices, 1);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0.0f, 0.0f, h);
  gluDisk(q, 0.0, r1, slices, 1);
  glPopMatrix();

  gluDeleteQuadric(q);
}

static void solidSphere(float r, int slices, int stacks) {
  GLUquadric* q = gluNewQuadric();
  gluQuadricNormals(q, GLU_SMOOTH);
  gluSphere(q, r, slices, stacks);
  gluDeleteQuadric(q);
}

void Renderer::drawPawn() const {
  glPushMatrix();
  // base
  glTranslatef(0.0f, 0.0f, 0.0f);
  glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
  solidCylinder(0.38f, 0.34f, 0.15f, 24);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0.0f, 0.15f, 0.0f);
  glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
  solidCylinder(0.28f, 0.18f, 0.45f, 24);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0.0f, 0.65f, 0.0f);
  solidSphere(0.20f, 24, 16);
  glPopMatrix();
}

void Renderer::drawRook() const {
  glPushMatrix();
  glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
  solidCylinder(0.42f, 0.38f, 0.18f, 24);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0.0f, 0.18f, 0.0f);
  glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
  solidCylinder(0.30f, 0.30f, 0.55f, 24);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0.0f, 0.73f, 0.0f);
  glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
  solidCylinder(0.35f, 0.35f, 0.10f, 24);
  glPopMatrix();

  // battlements
  glDisable(GL_CULL_FACE);
  glPushMatrix();
  glTranslatef(0.0f, 0.82f, 0.0f);
  for (int i = 0; i < 6; ++i) {
    float a = static_cast<float>(i) * 60.0f;
    float rad = a * 3.14159265f / 180.0f;
    glPushMatrix();
    glTranslatef(std::cos(rad) * 0.28f, 0.0f, std::sin(rad) * 0.28f);
    glScalef(0.10f, 0.12f, 0.10f);
    glBegin(GL_QUADS);
    // cube-ish
    glNormal3f(0, 1, 0);
    glVertex3f(-1, 1, -1); glVertex3f(1, 1, -1); glVertex3f(1, 1, 1); glVertex3f(-1, 1, 1);
    glNormal3f(0, -1, 0);
    glVertex3f(-1, -1, -1); glVertex3f(-1, -1, 1); glVertex3f(1, -1, 1); glVertex3f(1, -1, -1);
    glNormal3f(0, 0, 1);
    glVertex3f(-1, -1, 1); glVertex3f(-1, 1, 1); glVertex3f(1, 1, 1); glVertex3f(1, -1, 1);
    glNormal3f(0, 0, -1);
    glVertex3f(-1, -1, -1); glVertex3f(1, -1, -1); glVertex3f(1, 1, -1); glVertex3f(-1, 1, -1);
    glNormal3f(1, 0, 0);
    glVertex3f(1, -1, -1); glVertex3f(1, -1, 1); glVertex3f(1, 1, 1); glVertex3f(1, 1, -1);
    glNormal3f(-1, 0, 0);
    glVertex3f(-1, -1, -1); glVertex3f(-1, 1, -1); glVertex3f(-1, 1, 1); glVertex3f(-1, -1, 1);
    glEnd();
    glPopMatrix();
  }
  glPopMatrix();
  glEnable(GL_CULL_FACE);
}

void Renderer::drawKnight() const {
  // stylized horse head from scaled spheres + a "neck" cylinder
  glPushMatrix();
  glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
  solidCylinder(0.42f, 0.36f, 0.18f, 24);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0.0f, 0.18f, 0.0f);
  glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
  solidCylinder(0.30f, 0.22f, 0.55f, 24);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0.0f, 0.65f, 0.05f);
  glRotatef(-15.0f, 1.0f, 0.0f, 0.0f);
  glScalef(0.85f, 1.10f, 0.55f);
  solidSphere(0.28f, 24, 16);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0.0f, 0.88f, 0.18f);
  glRotatef(-35.0f, 1.0f, 0.0f, 0.0f);
  glScalef(0.70f, 0.95f, 0.45f);
  solidSphere(0.22f, 24, 16);
  glPopMatrix();

  // ears
  glPushMatrix();
  glTranslatef(0.12f, 1.02f, 0.20f);
  glRotatef(-35.0f, 1.0f, 0.0f, 0.0f);
  glRotatef(10.0f, 0.0f, 1.0f, 0.0f);
  glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
  solidCylinder(0.04f, 0.0f, 0.18f, 12);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(-0.12f, 1.02f, 0.20f);
  glRotatef(-35.0f, 1.0f, 0.0f, 0.0f);
  glRotatef(-10.0f, 0.0f, 1.0f, 0.0f);
  glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
  solidCylinder(0.04f, 0.0f, 0.18f, 12);
  glPopMatrix();
}

void Renderer::drawBishop() const {
  glPushMatrix();
  glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
  solidCylinder(0.40f, 0.34f, 0.18f, 24);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0.0f, 0.18f, 0.0f);
  glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
  solidCylinder(0.28f, 0.16f, 0.65f, 24);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0.0f, 0.82f, 0.0f);
  glScalef(0.85f, 1.10f, 0.85f);
  solidSphere(0.24f, 24, 16);
  glPopMatrix();

  // top knob
  glPushMatrix();
  glTranslatef(0.0f, 1.06f, 0.0f);
  solidSphere(0.10f, 20, 12);
  glPopMatrix();
}

void Renderer::drawQueen() const {
  glPushMatrix();
  glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
  solidCylinder(0.44f, 0.38f, 0.18f, 24);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0.0f, 0.18f, 0.0f);
  glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
  solidCylinder(0.30f, 0.18f, 0.75f, 28);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0.0f, 0.98f, 0.0f);
  solidSphere(0.22f, 28, 18);
  glPopMatrix();

  // crown points
  for (int i = 0; i < 8; ++i) {
    float a = static_cast<float>(i) * 45.0f;
    float rad = a * 3.14159265f / 180.0f;
    glPushMatrix();
    glTranslatef(std::cos(rad) * 0.22f, 1.15f, std::sin(rad) * 0.22f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    solidCylinder(0.03f, 0.0f, 0.18f, 10);
    glPopMatrix();
  }
}

void Renderer::drawKing() const {
  glPushMatrix();
  glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
  solidCylinder(0.46f, 0.40f, 0.18f, 24);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0.0f, 0.18f, 0.0f);
  glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
  solidCylinder(0.32f, 0.20f, 0.80f, 28);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0.0f, 1.02f, 0.0f);
  solidSphere(0.20f, 28, 18);
  glPopMatrix();

  // cross
  glDisable(GL_CULL_FACE);
  glPushMatrix();
  glTranslatef(0.0f, 1.26f, 0.0f);
  glScalef(0.06f, 0.18f, 0.06f);
  glBegin(GL_QUADS);
  glNormal3f(0, 1, 0);
  glVertex3f(-1, 1, -1); glVertex3f(1, 1, -1); glVertex3f(1, 1, 1); glVertex3f(-1, 1, 1);
  glNormal3f(0, -1, 0);
  glVertex3f(-1, -1, -1); glVertex3f(-1, -1, 1); glVertex3f(1, -1, 1); glVertex3f(1, -1, -1);
  glNormal3f(0, 0, 1);
  glVertex3f(-1, -1, 1); glVertex3f(-1, 1, 1); glVertex3f(1, 1, 1); glVertex3f(1, -1, 1);
  glNormal3f(0, 0, -1);
  glVertex3f(-1, -1, -1); glVertex3f(1, -1, -1); glVertex3f(1, 1, -1); glVertex3f(-1, 1, -1);
  glNormal3f(1, 0, 0);
  glVertex3f(1, -1, -1); glVertex3f(1, -1, 1); glVertex3f(1, 1, 1); glVertex3f(1, 1, -1);
  glNormal3f(-1, 0, 0);
  glVertex3f(-1, -1, -1); glVertex3f(-1, 1, -1); glVertex3f(-1, 1, 1); glVertex3f(-1, -1, 1);
  glEnd();
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0.0f, 1.26f, 0.0f);
  glScalef(0.18f, 0.06f, 0.06f);
  glBegin(GL_QUADS);
  glNormal3f(0, 1, 0);
  glVertex3f(-1, 1, -1); glVertex3f(1, 1, -1); glVertex3f(1, 1, 1); glVertex3f(-1, 1, 1);
  glNormal3f(0, -1, 0);
  glVertex3f(-1, -1, -1); glVertex3f(-1, -1, 1); glVertex3f(1, -1, 1); glVertex3f(1, -1, -1);
  glNormal3f(0, 0, 1);
  glVertex3f(-1, -1, 1); glVertex3f(-1, 1, 1); glVertex3f(1, 1, 1); glVertex3f(1, -1, 1);
  glNormal3f(0, 0, -1);
  glVertex3f(-1, -1, -1); glVertex3f(1, -1, -1); glVertex3f(1, 1, -1); glVertex3f(-1, 1, -1);
  glNormal3f(1, 0, 0);
  glVertex3f(1, -1, -1); glVertex3f(1, -1, 1); glVertex3f(1, 1, 1); glVertex3f(1, 1, -1);
  glNormal3f(-1, 0, 0);
  glVertex3f(-1, -1, -1); glVertex3f(-1, 1, -1); glVertex3f(-1, 1, 1); glVertex3f(-1, -1, 1);
  glEnd();
  glPopMatrix();

  glEnable(GL_CULL_FACE);
}

bool Renderer::screenToBoardSquare(double mouseX, double mouseY, int width, int height, Square& outSquare) const {
  // Ensure matrices are up-to-date.
  cacheMatrices();

  double x = mouseX;
  (void)width;
  (void)height;
  double y = static_cast<double>(viewport_[3]) - mouseY; // flip using viewport height

  double nearX, nearY, nearZ;
  double farX, farY, farZ;

  if (gluUnProject(x, y, 0.0, modelview_, projection_, viewport_, &nearX, &nearY, &nearZ) == GL_FALSE) return false;
  if (gluUnProject(x, y, 1.0, modelview_, projection_, viewport_, &farX, &farY, &farZ) == GL_FALSE) return false;

  Vec3 p0{static_cast<float>(nearX), static_cast<float>(nearY), static_cast<float>(nearZ)};
  Vec3 p1{static_cast<float>(farX), static_cast<float>(farY), static_cast<float>(farZ)};
  Vec3 d = p1 - p0;

  if (std::fabs(d.y) < 1e-6f) return false;
  float t = -p0.y / d.y;
  if (t < 0.0f || t > 1e6f) return false;

  Vec3 hit = p0 + d * t;

  // Map to board coordinates
  if (hit.x < -4.0f || hit.x >= 4.0f || hit.z < -4.0f || hit.z >= 4.0f) return false;

  int file = static_cast<int>(std::floor(hit.x + 4.0f));
  int rank = static_cast<int>(std::floor(4.0f - hit.z));

  if (file < 0 || file > 7 || rank < 0 || rank > 7) return false;

  outSquare = Square(file, rank);
  return true;
}
