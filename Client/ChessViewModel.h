#ifndef CHESSVIEWMODEL_H
#define CHESSVIEWMODEL_H
#include "Common/ChessField.h"
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class ChessViewModel : public QObject {
  Q_OBJECT
public:
  ChessViewModel();

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
  void check();
  void pawnHasReachedEnemysBase(int x, int y);

private:
  QTcpSocket *socket;
  QTcpServer *server;
};

#endif // CHESSVIEWMODEL_H
