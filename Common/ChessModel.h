#ifndef CHESSMODEL_H
#define CHESSMODEL_H
#include "Common/ChessField.h"
#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QPair>

class ChessModel : public QObject {
  Q_OBJECT
public:
  ChessModel();
  ChessModel(int fixedPlayerNumber);

  QList<QPair<int, int>> possibleSteps(int x, int y,
                                       bool includeDefendedPieces = false,
                                       bool attack = false,
                                       bool newTable = false);

  void newGame();
  void stepPiece(int from_x, int from_y, int to_x, int to_y);
  ChessField getField(int x, int y);
  void setHighlighted(int x, int y, bool highlight);
  void switchToQueen(int x, int y, PieceTypes switchTo);

  QJsonObject serializeTable();
  void deSerializeTable(QJsonObject tableJson);

signals:
  void gameOver(int player);
  void stepped(bool pieceKnockedDown);
  void check();
  void pawnHasReachedEnemysBase(int x, int y);
  void refreshTable();

private:
  QJsonObject serializeField(int x, int y);
  void deSerializeField(QJsonObject fieldJson, int x, int y);

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
  int fixedPlayerNumber_;
  bool isChecked = false;
  ChessField **chessTable_;
  ChessField **newTable_;
};

#endif // CHESSMODEL_H
