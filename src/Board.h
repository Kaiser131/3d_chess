#pragma once

#include "Piece.h"

#include <array>
#include <memory>
#include <optional>
#include <vector>

struct Move {
  Square from;
  Square to;
};

struct Board {
  // [file][rank]
  std::array<std::array<std::unique_ptr<Piece>, 8>, 8> pieces{};
  PieceColor sideToMove = PieceColor::White;

  Board();

  void reset();

  const Piece* at(Square s) const;
  Piece* at(Square s);

  bool isEmpty(Square s) const;
  bool isFriendly(Square s, PieceColor c) const;
  bool isEnemy(Square s, PieceColor c) const;

  std::vector<Square> legalMovesFrom(Square from) const;
  bool tryMakeMove(Square from, Square to);

  bool inBounds(Square s) const { return isOnBoard(s); }
};
