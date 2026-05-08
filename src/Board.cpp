#include "Board.h"

#include <algorithm>

Board::Board() { reset(); }

static std::unique_ptr<Piece> makePiece(PieceType t, PieceColor c) {
  switch (t) {
    case PieceType::King: return std::make_unique<KingPiece>(c);
    case PieceType::Queen: return std::make_unique<QueenPiece>(c);
    case PieceType::Rook: return std::make_unique<RookPiece>(c);
    case PieceType::Bishop: return std::make_unique<BishopPiece>(c);
    case PieceType::Knight: return std::make_unique<KnightPiece>(c);
    case PieceType::Pawn: return std::make_unique<PawnPiece>(c);
  }
  return nullptr;
}

void Board::reset() {
  for (auto& col : pieces) {
    for (auto& cell : col) cell.reset();
  }

  sideToMove = PieceColor::White;

  // White back rank (rank 0)
  pieces[0][0] = makePiece(PieceType::Rook, PieceColor::White);
  pieces[1][0] = makePiece(PieceType::Knight, PieceColor::White);
  pieces[2][0] = makePiece(PieceType::Bishop, PieceColor::White);
  pieces[3][0] = makePiece(PieceType::Queen, PieceColor::White);
  pieces[4][0] = makePiece(PieceType::King, PieceColor::White);
  pieces[5][0] = makePiece(PieceType::Bishop, PieceColor::White);
  pieces[6][0] = makePiece(PieceType::Knight, PieceColor::White);
  pieces[7][0] = makePiece(PieceType::Rook, PieceColor::White);
  for (int f = 0; f < 8; ++f) pieces[f][1] = makePiece(PieceType::Pawn, PieceColor::White);

  // Black back rank (rank 7)
  pieces[0][7] = makePiece(PieceType::Rook, PieceColor::Black);
  pieces[1][7] = makePiece(PieceType::Knight, PieceColor::Black);
  pieces[2][7] = makePiece(PieceType::Bishop, PieceColor::Black);
  pieces[3][7] = makePiece(PieceType::Queen, PieceColor::Black);
  pieces[4][7] = makePiece(PieceType::King, PieceColor::Black);
  pieces[5][7] = makePiece(PieceType::Bishop, PieceColor::Black);
  pieces[6][7] = makePiece(PieceType::Knight, PieceColor::Black);
  pieces[7][7] = makePiece(PieceType::Rook, PieceColor::Black);
  for (int f = 0; f < 8; ++f) pieces[f][6] = makePiece(PieceType::Pawn, PieceColor::Black);
}

const Piece* Board::at(Square s) const {
  if (!inBounds(s)) return nullptr;
  return pieces[s.file][s.rank].get();
}

Piece* Board::at(Square s) {
  if (!inBounds(s)) return nullptr;
  return pieces[s.file][s.rank].get();
}

bool Board::isEmpty(Square s) const { return at(s) == nullptr; }

bool Board::isFriendly(Square s, PieceColor c) const {
  const Piece* p = at(s);
  return p && p->color == c;
}

bool Board::isEnemy(Square s, PieceColor c) const {
  const Piece* p = at(s);
  return p && p->color != c;
}

std::vector<Square> Board::legalMovesFrom(Square from) const {
  const Piece* p = at(from);
  if (!p) return {};
  if (p->color != sideToMove) return {};
  return p->generateMoves(*this, from);
}

bool Board::tryMakeMove(Square from, Square to) {
  auto moves = legalMovesFrom(from);
  auto it = std::find(moves.begin(), moves.end(), to);
  if (it == moves.end()) return false;

  // capture (overwrite)
  pieces[to.file][to.rank] = std::move(pieces[from.file][from.rank]);

  // pawn promotion (auto queen)
  if (pieces[to.file][to.rank] && pieces[to.file][to.rank]->type == PieceType::Pawn) {
    PieceColor c = pieces[to.file][to.rank]->color;
    if ((c == PieceColor::White && to.rank == 7) || (c == PieceColor::Black && to.rank == 0)) {
      pieces[to.file][to.rank] = makePiece(PieceType::Queen, c);
    }
  }

  sideToMove = (sideToMove == PieceColor::White) ? PieceColor::Black : PieceColor::White;
  return true;
}
