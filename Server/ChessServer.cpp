#include "ChessServer.h"

ChessServer::ChessServer(QObject *parent) : QObject(parent) {
  model_ = new ChessModel();
  server_ = new QTcpServer(this);

  connect(server_, &QTcpServer::newConnection, this,
          &ChessServer::onNewConnection);

  // port_ = 1337
  if (!server_->listen(QHostAddress::Any, port_)) {
    qDebug() << "Server could not start";
  } else {
    qDebug() << "Server is listening on port " << port_;
  }
}

void ChessServer::onGameOver(int Player) {}
void ChessServer::onCheck() {}
void ChessServer::onNewConnection() {
  QTcpSocket *socket = server_->nextPendingConnection();
  connect(socket, &QTcpSocket::readyRead, this, [=]() {
    auto data = socket->readAll();
    qDebug() << data;
  });
  qDebug() << "Localaddress: " << socket->localAddress()
           << ", peerAddress: " << socket->peerAddress();
  sockets_[socket->peerAddress().toString()] = socket;
}
