#ifndef CHESSAPISERVICE_H
#define CHESSAPISERVICE_H
#include "ChessField.h"
#include "backend/ChessModel.h"
#include "backend/IChessModel.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <mutex>

class ChessAPIService : public IChessModel {
  Q_OBJECT
public:
  ChessAPIService();
  ~ChessAPIService();
  // ---- API FUNCTIONS ----------------------------------------------
  void initSockets();
  void closeSockets();
  void loginToServer(QString username, QString password);
  void signUpToServer(QString email, QString username, QString password);
  void startQueueing();
  void stopQueueing();
  void endGameSession(bool logout);
  void setNetworkValues(QString serverAddress, int requestPort,
                        int responsePort, int heartbeatPort);
  void logOut();

  bool getInGame();
  bool getInQueue();
  int getElo();
  QString getUsername();

  int getOpponentsElo();
  QString getOpponentsUsername();

  // ---- MODEL FUNCTIONS --------------------------------------------
  QList<QPair<int, int>> possibleSteps(int x, int y,
                                       bool includeDefendedPieces = false,
                                       bool attack = false,
                                       bool newTable = false) override;

  void stepPiece(int from_x, int from_y, int to_x, int to_y) override;
  void newGame() override;
  ChessField getField(int x, int y) override;
  void setHighlighted(int x, int y, bool highlight) override;
  void switchToQueen(int x, int y, PieceTypes switchTo) override;
  int getCurrentPlayer() override;
  bool isMyPiece(int x, int y) override;
signals:
  void startGame(int fixedPlayerNumber);
  void connectedToServer(bool success);
  void loginSuccess(bool success, QString message);
  void createSuccess(bool success, QString message);
  void gameEnded(int newElo);
  void serverTimedOut();

private:
  void sendRequest(QJsonObject request);
  void handleGameOver(int winner);

  // QPair<SessionID, p1/p2>
  QPair<QString, int> gameSessionID_;

  QString userSessionID_ = "";
  QString userName_ = "";
  int elo_ = -1;
  QString opponentsUsername_ = "";
  int opponentsElo_ = -1;

  bool inGame_ = false;
  bool inQueue_ = false;
  bool pieceSwitched_ = false;
  PieceTypes pieceSwitchedType_ = PieceTypes::VoidType;

  ChessModel *model_;
  QTcpSocket *requestSocket_ = nullptr;
  QTcpServer *responseServer_ = nullptr;
  QTcpSocket *responseSocket_ = nullptr;
  QTcpServer *heartbeatServer_ = nullptr;
  QTcpSocket *heartbeatSocket_ = nullptr;
  QString serverAddress_ = "127.0.0.1";
  int requestPort_ = 1337;
  int responsePort_ = 1338;
  int heartbeatPort_ = 1339;
  QTimer *heartbeatTimer_ = nullptr;
  std::mutex heartbeatMutex_;
  bool heartBeated_ = true;
};

#endif // CHESSAPISERVICE_H
