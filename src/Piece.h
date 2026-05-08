#pragma once

#include <vector>

struct Board;

enum class PieceColor { White, Black };

enum class PieceType {
  King,
  Queen,
  Rook,
  Bishop,
  Knight,
  Pawn,
};

struct Square {
  int file = 0; // 0..7 (a..h)
  int rank = 0; // 0..7 (white home rank = 0)

  bool operator==(const Square& o) const { return file == o.file && rank == o.rank; }
};

inline bool isOnBoard(const Square& s) {
  return s.file >= 0 && s.file < 8 && s.rank >= 0 && s.rank < 8;
}

struct Piece {
  PieceType type;
  PieceColor color;

  explicit Piece(PieceType t, PieceColor c) : type(t), color(c) {}
  virtual ~Piece() = default;

  virtual std::vector<Square> generateMoves(const Board& board, Square from) const = 0;
};

struct KingPiece final : Piece {
  explicit KingPiece(PieceColor c) : Piece(PieceType::King, c) {}
  std::vector<Square> generateMoves(const Board& board, Square from) const override;
};

struct QueenPiece final : Piece {
  explicit QueenPiece(PieceColor c) : Piece(PieceType::Queen, c) {}
  std::vector<Square> generateMoves(const Board& board, Square from) const override;
};

struct RookPiece final : Piece {
  explicit RookPiece(PieceColor c) : Piece(PieceType::Rook, c) {}
  std::vector<Square> generateMoves(const Board& board, Square from) const override;
};

struct BishopPiece final : Piece {
  explicit BishopPiece(PieceColor c) : Piece(PieceType::Bishop, c) {}
  std::vector<Square> generateMoves(const Board& board, Square from) const override;
};

struct KnightPiece final : Piece {
  explicit KnightPiece(PieceColor c) : Piece(PieceType::Knight, c) {}
  std::vector<Square> generateMoves(const Board& board, Square from) const override;
};

struct PawnPiece final : Piece {
  explicit PawnPiece(PieceColor c) : Piece(PieceType::Pawn, c) {}
  std::vector<Square> generateMoves(const Board& board, Square from) const override;
};
