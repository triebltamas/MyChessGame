#ifndef CHESSMODEL_H
#define CHESSMODEL_H
#include "ChessField.h"
#include <QList>
#include <QObject>
#include <QPair>

class ChessModel : public QObject {
  Q_OBJECT
public:
  ChessModel();

  QList<QPair<int, int>> possibleSteps(int x, int y,
                                       bool includeDefendedPieces = false,
                                       bool attack = false,
                                       bool newTable = false);

  void stepPiece(int from_x, int from_y, int to_x, int to_y);
  void newGame();
  ChessField getField(int x, int y);
  void setHighlighted(int x, int y, bool highlight);
  void switchToQueen(int x, int y, PieceTypes switchTo);

signals:
  void gameOver(int player);
  void stepped(bool pieceKnockedDown);
  void check();
  void pawnHasReachedEnemysBase(int x, int y);
  void refreshTable();

private:
  bool checkGameOver();
  bool stepCausesSelfCheck(int from_x, int from_y, int to_x, int to_y,
                           bool attack);
  bool isSamePieceColor(int x, int y, PieceColor color, bool newTable,
                        bool includeDefendedPieces);

  QList<QPair<int, int>>
  possibleStepsForKing(int x, int y, PieceColor color,
                       bool includeDefendedPieces = false, bool attack = false,
                       bool newTable = false);
  QList<QPair<int, int>>
  possibleStepsForQueen(int x, int y, PieceColor color,
                        bool includeDefendedPieces = false, bool attack = false,
                        bool newTable = false);
  QList<QPair<int, int>>
  possibleStepsForRook(int x, int y, PieceColor color,
                       bool includeDefendedPieces = false, bool attack = false,
                       bool newTable = false);
  QList<QPair<int, int>>
  possibleStepsForBishup(int x, int y, PieceColor color,
                         bool includeDefendedPieces = false,
                         bool attack = false, bool newTable = false);
  QList<QPair<int, int>>
  possibleStepsForKnight(int x, int y, PieceColor color,
                         bool includeDefendedPieces = false,
                         bool attack = false, bool newTable = false);
  QList<QPair<int, int>>
  possibleStepsForPawn(int x, int y, PieceColor color,
                       bool includeDefendedPieces = false, bool attack = false,
                       bool newTable = false);

  int N_;
  int currentPlayer_; // 1 or 2 or 0 if draw
  bool isChecked = false;
  ChessField **chessTable_;
  ChessField **newTable_;
};

#endif // CHESSMODEL_H
