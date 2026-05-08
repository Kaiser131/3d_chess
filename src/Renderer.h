#pragma once

#include "Board.h"
#include "Camera.h"

#include <vector>

struct RenderSelection {
  bool hasSelected = false;
  Square selected{};
  std::vector<Square> validMoves;
};

struct Renderer {
  float tileSize = 1.0f;

  void initGL();
  void resize(int width, int height);

  void beginFrame(const Camera& camera, int width, int height);
  void drawScene(const Board& board, const RenderSelection& selection);
  void endFrame();

  // Picking helpers (use after beginFrame() so matrices are current)
  bool screenToBoardSquare(double mouseX, double mouseY, int width, int height, Square& outSquare) const;

private:
  // Cached matrices for gluUnProject
  mutable int viewport_[4]{};
  mutable double modelview_[16]{};
  mutable double projection_[16]{};

  void cacheMatrices() const;

  void drawBoard(const RenderSelection& selection);
  void drawPieces(const Board& board, const RenderSelection& selection);

  void setMaterialForTile(bool dark, bool highlight) const;
  void setMaterialForPiece(PieceColor color, bool selected) const;

  void drawTile(int file, int rank, bool dark, bool highlight) const;
  void drawHighlightMarker(int file, int rank, bool isCapture) const;

  void drawPieceModel(PieceType type) const;
  void drawPawn() const;
  void drawRook() const;
  void drawKnight() const;
  void drawBishop() const;
  void drawQueen() const;
  void drawKing() const;
};
