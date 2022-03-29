#ifndef CHESSSERVER_H
#define CHESSSERVER_H

#include "Common/ChessField.h"
#include "Common/ChessModel.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class ChessServer : public QObject {
  Q_OBJECT
public:
  ChessServer(QObject *parent = nullptr);
  ~ChessServer();
public slots:
  void onGameOver(int Player);
  void onCheck();
  void onNewConnection();

private:
  ChessModel *model_ = nullptr;
  QTcpServer *server_ = nullptr;
  QMap<QString, QTcpSocket *> requestSockets_;
  QMap<QString, QTcpSocket *> responseSockets_;
  int requestPort_ = 1337;
  int responsePort_ = 1338;
};

#endif // CHESSSERVER_H
