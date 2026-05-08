#include "Piece.h"
#include "Board.h"

#include <array>

static void addIfValid(std::vector<Square>& out, const Board& board, Square to, PieceColor mover) {
  if (!isOnBoard(to)) return;
  if (board.isFriendly(to, mover)) return;
  out.push_back(to);
}

static void addSlides(std::vector<Square>& out, const Board& board, Square from, PieceColor mover, int df, int dr) {
  Square cur = from;
  while (true) {
    cur = Square(cur.file + df, cur.rank + dr);
    if (!isOnBoard(cur)) break;
    if (board.isEmpty(cur)) {
      out.push_back(cur);
      continue;
    }
    if (board.isEnemy(cur, mover)) out.push_back(cur);
    break;
  }
}

std::vector<Square> KingPiece::generateMoves(const Board& board, Square from) const {
  std::vector<Square> m;
  for (int df = -1; df <= 1; ++df) {
    for (int dr = -1; dr <= 1; ++dr) {
      if (df == 0 && dr == 0) continue;
      addIfValid(m, board, Square(from.file + df, from.rank + dr), color);
    }
  }
  return m;
}

std::vector<Square> QueenPiece::generateMoves(const Board& board, Square from) const {
  std::vector<Square> m;
  static const std::array<std::pair<int, int>, 8> dirs = {{
      std::make_pair(1, 0), std::make_pair(-1, 0), std::make_pair(0, 1), std::make_pair(0, -1),
      std::make_pair(1, 1), std::make_pair(1, -1), std::make_pair(-1, 1), std::make_pair(-1, -1),
  }};
  for (size_t i = 0; i < dirs.size(); ++i) {
    addSlides(m, board, from, color, dirs[i].first, dirs[i].second);
  }
  return m;
}

std::vector<Square> RookPiece::generateMoves(const Board& board, Square from) const {
  std::vector<Square> m;
  static const std::array<std::pair<int, int>, 4> dirs = {{
      std::make_pair(1, 0), std::make_pair(-1, 0), std::make_pair(0, 1), std::make_pair(0, -1),
  }};
  for (size_t i = 0; i < dirs.size(); ++i) {
    addSlides(m, board, from, color, dirs[i].first, dirs[i].second);
  }
  return m;
}

std::vector<Square> BishopPiece::generateMoves(const Board& board, Square from) const {
  std::vector<Square> m;
  static const std::array<std::pair<int, int>, 4> dirs = {{
      std::make_pair(1, 1), std::make_pair(1, -1), std::make_pair(-1, 1), std::make_pair(-1, -1),
  }};
  for (size_t i = 0; i < dirs.size(); ++i) {
    addSlides(m, board, from, color, dirs[i].first, dirs[i].second);
  }
  return m;
}

std::vector<Square> KnightPiece::generateMoves(const Board& board, Square from) const {
  std::vector<Square> m;
  static constexpr std::array<std::pair<int, int>, 8> k = {{{1, 2}, {2, 1}, {-1, 2}, {-2, 1}, {1, -2}, {2, -1}, {-1, -2}, {-2, -1}}};
  for (size_t i = 0; i < k.size(); ++i) {
    int df = k[i].first;
    int dr = k[i].second;
    addIfValid(m, board, Square(from.file + df, from.rank + dr), color);
  }
  return m;
}

std::vector<Square> PawnPiece::generateMoves(const Board& board, Square from) const {
  std::vector<Square> m;
  int dir = (color == PieceColor::White) ? 1 : -1;

  Square one(from.file, from.rank + dir);
  if (isOnBoard(one) && board.isEmpty(one)) {
    m.push_back(one);

    bool onStart = (color == PieceColor::White && from.rank == 1) || (color == PieceColor::Black && from.rank == 6);
    if (onStart) {
      Square two(from.file, from.rank + 2 * dir);
      if (isOnBoard(two) && board.isEmpty(two)) m.push_back(two);
    }
  }

  Square capL(from.file - 1, from.rank + dir);
  Square capR(from.file + 1, from.rank + dir);
  if (isOnBoard(capL) && board.isEnemy(capL, color)) m.push_back(capL);
  if (isOnBoard(capR) && board.isEnemy(capR, color)) m.push_back(capR);

  return m;
}
