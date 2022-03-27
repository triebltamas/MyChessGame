#include "ChessServer.h"

ChessServer::ChessServer(QObject *parent) : QObject(parent) {
  model_ = new ChessModel();
  server_ = new QTcpServer();
  socket_ = new QTcpSocket();
}
void ChessServer::run() {
  // start listening
}

void ChessServer::onGameOver(int Player) {}
void ChessServer::onCheck() {}
