#ifndef CHESSAPISERVICE_H
#define CHESSAPISERVICE_H
#include "common/ChessField.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <mutex>

class ChessAPIService : public QObject {
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
  QPair<QString, int> getGameSessionID();

  int getOpponentsElo();
  QString getOpponentsUsername();

  void onPieceSwitched(PieceTypes switchTo = PieceTypes::VoidType);
  void sendRequest(QJsonObject request);

  // ---- MODEL FUNCTIONS --------------------------------------------
  void stepPiece(int from_x, int from_y, int to_x, int to_y);
signals:
  void startGame(int fixedPlayerNumber);
  void connectedToServer(bool success);
  void loginSuccess(bool success, QString message);
  void createSuccess(bool success, QString message);
  void gameEnded(int newElo);
  void gameOver(int player, int newElo, bool disconnect = false);
  void serverTimedOut();
  void opponentStepped(int from_x, int from_y, int to_x, int to_y);
  void opponentSwitched(int x, int y, PieceTypes piecetype);

private:
  void handleGameOver(int winner);

  QString salt_ = "287gf81RG^&@%FV8F*)81";

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
