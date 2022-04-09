#ifndef CHESSAPISERVICE_H
#define CHESSAPISERVICE_H
#include "ChessField.h"
#include "ChessModel.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class ChessAPIService : public QObject {
  Q_OBJECT
public:
  ChessAPIService();
  ~ChessAPIService();
  void initSockets();
  void closeSockets();
  void loginToServer(QString username, QString password);
  void signUpToServer(QString email, QString username, QString password);
  void startQueueing();
  void endGameSession();
  void setNetworkValues(QString serverAddress, int requestPort,
                        int responsePort);

  QList<QPair<int, int>> possibleSteps(int x, int y,
                                       bool includeDefendedPieces = false,
                                       bool attack = false,
                                       bool newTable = false);

  void stepPiece(int from_x, int from_y, int to_x, int to_y);
  void newGame();
  ChessField getField(int x, int y);
  void setHighlighted(int x, int y, bool highlight);
  void switchToQueen(int x, int y, PieceTypes switchTo);
  int getCurrentPlayer();
  bool getInGame();
signals:
  void gameOver(int player);
  void check();
  void pawnHasReachedEnemysBase(int x, int y);
  void startGame(int fixedPlayerNumber);
  void connectedToServer(bool success);
  void loginSuccess(bool success, QString message);
  void createSuccess(bool success, QString message);
  void refreshTable();
  void gameEnded(QString message);

private:
  void sendRequest(QJsonObject request);
  //  void sendTable();
  void sendFields(QList<QPair<int, int>> fields);
  void handleGameOver(int winner);

  // QPair<SessionID, p1/p2>
  QPair<QString, int> gameSessionID_;

  QString userSessionID_ = "";
  bool inGame_ = false;
  bool pieceSwitched_ = false;
  PieceTypes pieceSwitchedType_ = PieceTypes::VoidType;

  ChessModel *model_;
  QTcpSocket *requestSocket_ = nullptr;
  QTcpServer *responseServer_ = nullptr;
  QTcpSocket *responseSocket_ = nullptr;
  QString serverAddress_ = "127.0.0.1";
  int requestPort_ = 1337;
  int responsePort_ = 1338;
};

#endif // CHESSAPISERVICE_H
