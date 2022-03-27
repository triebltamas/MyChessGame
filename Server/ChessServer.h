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
  void run();

public slots:
  void onGameOver(int Player);
  void onCheck();

private:
  ChessModel *model_ = nullptr;
  QTcpServer *server_ = nullptr;
  QTcpSocket *socket_ = nullptr;
};

#endif // CHESSSERVER_H
