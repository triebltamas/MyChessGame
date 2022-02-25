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

signals:
  void gameOver(int player);
  void stepped(bool pieceKnockedDown);
  void check();
  void pawnHasReachedEnemysBase(int pos);
  void refreshTable();

private:
  void switchToQueen(int pos, PieceTypes switchTo);
  bool checkGameOver();
  bool stepCausesSelfCheck(int from_x, int from_y, int to_x, int to_y,
                           bool attack);
  bool isValidStep(int from_x, int from_y, int to_x, int to_y);
  bool isSamePieceColor(int x, int y, PieceColor color, bool newTable,
                        bool includeDefendedPieces);
  QPair<int, int> getSingleChecker();

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

  int _N;
  int _currentPlayer; // 1 or 2
  ChessField **_chessTable;
  ChessField **_newTable;
};

#endif // CHESSMODEL_H
