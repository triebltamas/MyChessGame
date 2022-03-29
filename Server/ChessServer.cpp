#include "ChessServer.h"

ChessServer::ChessServer(QObject *parent) : QObject(parent) {
  model_ = new ChessModel();
  server_ = new QTcpServer(this);

  connect(server_, &QTcpServer::newConnection, this,
          &ChessServer::onNewConnection);

  // port_ = 1337
  if (!server_->listen(QHostAddress::Any, requestPort_)) {
    qDebug() << "Server could not start";
  } else {
    qDebug() << "Server is listening on port " << requestPort_;
  }
}

ChessServer::~ChessServer() {
  for (auto socket : requestSockets_) {
    if (socket->isOpen()) {
      qDebug() << "closing socket with peerAddress: " << socket->peerAddress();
      socket->close();
      socket->deleteLater();
    }
  }
  if (server_->isListening()) {
    qDebug() << "Closing server";
    server_->close();
  }

  delete model_;
}

void ChessServer::onGameOver(int Player) {}
void ChessServer::onCheck() {}
void ChessServer::onNewConnection() {
  QTcpSocket *requestSocket = server_->nextPendingConnection();

  if (requestSockets_.contains(requestSocket->peerAddress().toString())) {
    return;
    // maybe delete socket here
  }

  connect(requestSocket, &QTcpSocket::disconnected, this, [=]() {
    if (requestSockets_.contains(requestSocket->peerAddress().toString()))
      requestSockets_.remove(requestSocket->peerAddress().toString());
  });

  connect(requestSocket, &QTcpSocket::readyRead, this, [=]() {
    auto data = requestSocket->readAll();
    qDebug() << data;

    QString str = QString(data);
    str = str.replace("\n", "");
    QJsonDocument itemDoc = QJsonDocument::fromJson(str.toUtf8());
    QJsonObject request = itemDoc.object();
    QString func = request["Function"].toString();
    auto tableJson = request["Parameters"].toObject()["Table"].toObject();

    if (func == "deSerializeTable") {
      // feldolgozni a kerest es/vagy tovabbitani a masik kliensnek
    }
  });
  qDebug() << "Localaddress: " << requestSocket->localAddress().toString()
           << ", peerAddress: " << requestSocket->peerAddress().toString();

  requestSockets_[requestSocket->peerAddress().toString()] = requestSocket;

  QTcpSocket *responseSocket = new QTcpSocket(this);
  responseSocket->connectToHost(requestSocket->peerAddress(), responsePort_);
  if (!responseSocket->waitForConnected(3000)) {
    qDebug() << "Successfully connected to host on IP: "
             << requestSocket->peerAddress().toString()
             << " and port: " << responsePort_;
  } else {
    qDebug() << "Failed to connect to host on IP: "
             << requestSocket->peerAddress().toString()
             << " and port: " << responsePort_;
  }

  connect(responseSocket, &QTcpSocket::disconnected, this, [=]() {
    if (responseSockets_.contains(responseSocket->peerAddress().toString()))
      responseSockets_.remove(responseSocket->peerAddress().toString());
  });

  responseSockets_[responseSocket->peerAddress().toString()] = responseSocket;
}
