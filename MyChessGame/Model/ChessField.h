#ifndef CHESSFIELD_H
#define CHESSFIELD_H

enum class FieldColor { White, Black };
enum class PieceColor { VoidColor, White, Black };
enum class PieceTypes { VoidType, King, Queen, Rook, Bishup, Knight, Pawn };

typedef struct ChessField {
  FieldColor _fieldColor;
  PieceColor _pieceColor;
  PieceTypes _pieceType;
  bool highlighted = false;
} ChessField;

#endif // CHESSFIELD_H
