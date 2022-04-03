#ifndef CHESSSERVER_H
#define CHESSSERVER_H

#include "ChessField.h"
#include "ChessModel.h"
#include "GameSession.h"
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
  void onGameOver(QString sessionID, int sessionPlayer, int winnerPlayer);
  void onCheck(QString sessionID, int sessionPlayer);
  void onNewConnection();

private:
  void onDisconnected(QString key);
  void onResponseSockectAvailable(QHostAddress address, int responsePort,
                                  QTcpSocket *requestSocket);

  QTcpServer *server_ = nullptr;
  QMap<QString, GameSession> sessions_;
  QMap<QString, QString> sessionIDs_;
  int requestPort_ = 1337;
  int responsePort_ = 1338;
  QRandomGenerator *randomGenerator;
};

#endif // CHESSSERVER_H
