#ifndef CHESSSERVER_H
#define CHESSSERVER_H

#include "ChessField.h"
#include "ChessModel.h"
#include "DatabaseHandler.h"
#include "GameSession.h"
#include "UserSession.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QRandomGenerator>
#include <QTcpServer>
#include <QTcpSocket>

class ChessServer : public QObject {
  Q_OBJECT
public:
  ChessServer(QObject *parent = nullptr);
  ~ChessServer();
public slots:
  void onGameOver(QString sessionID, int winnerPlayer);
  void onCheck(QString sessionID, int sessionPlayer);
  void onNewConnection();

private:
  void onDisconnected(QString key);
  void onResponseSockectAvailable(QHostAddress address, int responsePort,
                                  QTcpSocket *requestSocket);
  void onStartQueueing(QString sessionID);

  void endGameSession(QString userSessionID);

  void loginUser(QString userSessionID, QString username, QString password);
  void createUser(QString userSessionID, QString email, QString username,
                  QString password);
  float getProbability(int rating1, int rating2);
  QPair<int, int> getNewEloRating(float player1, float player2,
                                  int winnerPlayer);

  QTcpServer *server_ = nullptr;
  QRandomGenerator *randomGenerator_ = nullptr;
  DatabaseHandler *databaseHandler_ = nullptr;
  QMap<QString, GameSession> gameSessions_;
  QMap<QString, UserSession> userSessions_;
  int requestPort_ = 1337;
  int responsePort_ = 1338;
  int constant_ = 30;
};

#endif // CHESSSERVER_H
