#include "Ayyoo.h"

Ayyoo::Ayyoo() : model_(new ChessModel()) {
  requestSocket_ = new QTcpSocket(this);
  responseServer_ = new QTcpServer(this);

  // STARTING VIEWMODEL'S RESPONSE SERVER
  if (!responseServer_->listen(QHostAddress::Any, responsePort_)) {
    qDebug() << "Response server could not start";
  } else {
    qDebug() << "Response server is listening on port " << responsePort_;
  }

  // CONNECTING TO CHESSSERVER'S REQUEST SERVER
  requestSocket_->connectToHost(hostIP_, requestPort_);
  if (requestSocket_->waitForConnected(3000)) {
    qDebug() << "Successfully connected to host on IP: " << hostIP_
             << " and port: " << requestPort_;
    emit connected(true);
  } else {
    qDebug() << "Failed to connect to host on IP: " << hostIP_
             << " and port: " << requestPort_;
    emit connected(false);
  }

  connect(responseServer_, &QTcpServer::newConnection, this, [this]() {
    responseSocket_ = responseServer_->nextPendingConnection();
    qDebug() << "Successful connection to response server from ip:"
             << responseSocket_->peerAddress().toString()
             << " and port: " << responsePort_;
  });
}

Ayyoo::~Ayyoo() {
  if (requestSocket_->isOpen()) {
    requestSocket_->close();
    qDebug() << "socket closed";
  }
}

void Ayyoo::sendRequest(QJsonObject request) {
  if (!requestSocket_->isWritable()) {
    qWarning() << "Socket is not writeable!";
    return;
  }

  QJsonDocument doc(request);
  QByteArray data;
  data.append(QString::fromLatin1(doc.toJson()));
  requestSocket_->write(data);
  requestSocket_->waitForBytesWritten(1000);
}

QList<QPair<int, int>> Ayyoo::possibleSteps(int x, int y,
                                            bool includeDefendedPieces,
                                            bool attack, bool newTable) {
  return model_->possibleSteps(x, y, includeDefendedPieces, attack, newTable);
}

void Ayyoo::stepPiece(int from_x, int from_y, int to_x, int to_y) {
  model_->stepPiece(from_x, from_y, to_x, to_y);

  QJsonObject request = {
      {"Function", "deSerializeTable"},
      {"Parameters", QJsonObject{{"Table", model_->serializeTable()}}}};
  sendRequest(request);
}
void Ayyoo::newGame() { model_->newGame(); }

ChessField Ayyoo::getField(int x, int y) { return model_->getField(x, y); }
void Ayyoo::setHighlighted(int x, int y, bool highlight) {
  model_->setHighlighted(x, y, highlight);
}
void Ayyoo::switchToQueen(int x, int y, PieceTypes switchTo) {
  switchToQueen(x, y, switchTo);
}
