#include "ChessViewModel.h"

ChessViewModel::ChessViewModel() {}
QList<QPair<int, int>> ChessViewModel::possibleSteps(int x, int y,
                                                     bool includeDefendedPieces,
                                                     bool attack,
                                                     bool newTable) {
  return {};
}

void ChessViewModel::stepPiece(int from_x, int from_y, int to_x, int to_y) {}
void ChessViewModel::newGame() {}
ChessField ChessViewModel::getField(int x, int y) {
  ChessField f;
  return f;
}
void ChessViewModel::setHighlighted(int x, int y, bool highlight) {}
void ChessViewModel::switchToQueen(int x, int y, PieceTypes switchTo) {}
