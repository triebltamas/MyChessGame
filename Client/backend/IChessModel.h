#ifndef ICHESSMODEL_H
#define ICHESSMODEL_H
#include "common/ChessField.h"
#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QPair>

class IChessModel : public QObject {
  Q_OBJECT
public:
  virtual QList<QPair<int, int>>
  possibleSteps(int x, int y, bool includeDefendedPieces = false,
                bool attack = false, bool newTable = false) = 0;

  virtual void newGame() = 0;
  virtual void stepPiece(int from_x, int from_y, int to_x, int to_y) = 0;
  virtual ChessField getField(int x, int y) = 0;
  virtual void setHighlighted(int x, int y, bool highlight) = 0;
  virtual void switchToQueen(int x, int y, PieceTypes switchTo) = 0;
  virtual int getCurrentPlayer() = 0;
  virtual bool isMyPiece(int x, int y) = 0;

signals:
  void gameOver(int player, int newElo, bool disconnect = false);
  void check();
  void pawnHasReachedEnemysBase(int x, int y);
  void refreshTable();
  void currentPlayerChanged(int player);
};

#endif // ICHESSMODEL_H
