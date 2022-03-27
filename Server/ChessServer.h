#ifndef CHESSSERVER_H
#define CHESSSERVER_H

#include "ChessModel.h"
#include "Common/ChessField.h"
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class ChessServer : public QObject {
  Q_OBJECT
public:
  ChessServer(QObject *parent = nullptr);
  virtual ~ChessServer() {}

public slots:
  void onGameOver(int Player);
  void onCheck();
  void onNewConnection();

private:
  ChessModel *model_ = nullptr;
  QTcpServer *server_ = nullptr;
  QMap<QString, QTcpSocket *> sockets_;
  int port_ = 1337;
};

#endif // CHESSSERVER_H
