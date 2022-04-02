#include "ChessAPIService.h"

ChessAPIService::ChessAPIService() : model_(new ChessModel()) {
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
  } else {
    qDebug() << "Failed to connect to host on IP: " << hostIP_
             << " and port: " << requestPort_;
  }

  connect(responseServer_, &QTcpServer::newConnection, this, [this]() {
    responseSocket_ = responseServer_->nextPendingConnection();
    qDebug() << "Successful connection to response server from ip:"
             << responseSocket_->peerAddress().toString()
             << " and port: " << responsePort_;
  });

  // CONNECTIONS

  connect(responseSocket_, &QTcpSocket::readyRead, this, [this]() {
    auto data = responseSocket_->readAll();
    //    qDebug() << data;

    QString str = QString(data);
    str = str.replace("\n", "");
    QJsonDocument itemDoc = QJsonDocument::fromJson(str.toUtf8());
    QJsonObject request = itemDoc.object();
    QJsonObject parameters = request["Parameters"].toObject();
    QString func = request["Function"].toString();

    if (func == "connected") {
      gameSessionID.first = parameters["SessionID"].toString();
      gameSessionID.second = parameters["SessionPlayerNumber"].toInt();
      emit connected(gameSessionID.second);
    } else if (func == "startGame") {
      emit startGame();
    } else if (func == "deSerializeTable") {
      model_->deSerializeTable(parameters["Table"].toObject());
    } else if (func == "gameOver") {
      int winner = parameters["Player"].toInt();
      emit gameOver(winner);
      // todo maybe csakha kileptek a menube akkor kene ezt
      requestSocket_->write("deleteSession");
      requestSocket_->waitForBytesWritten(1000);
    } else if (func == "check") {
      emit check();
    }
  });

  connect(model_, &ChessModel::check, this, [this]() {
    QJsonObject request = {{"Function", "check"}};
    sendRequest(request);
  });

  connect(model_, &ChessModel::gameOver, this, [this](int Player) {
    QJsonObject request = {{"Function", "gameOver"},
                           {"Parameters", QJsonObject{{"Player", Player}}}};
    sendRequest(request);
  });
  connect(model_, &ChessModel::refreshTable, this,
          &ChessAPIService::refreshTable);
}

ChessAPIService::~ChessAPIService() {
  if (requestSocket_->isOpen()) {
    requestSocket_->close();
    qDebug() << "socket closed";
  }
}

void ChessAPIService::sendRequest(QJsonObject request) {
  if (!requestSocket_->isWritable()) {
    qWarning() << "Socket is not writeable!";
    return;
  }

  request.insert("SessionID", gameSessionID.first);
  request.insert("SessionPlayer", gameSessionID.second);

  QJsonDocument doc(request);
  QByteArray data;
  data.append(QString::fromLatin1(doc.toJson()));
  requestSocket_->write(data);
  requestSocket_->waitForBytesWritten(1000);
}

QList<QPair<int, int>>
ChessAPIService::possibleSteps(int x, int y, bool includeDefendedPieces,
                               bool attack, bool newTable) {
  return model_->possibleSteps(x, y, includeDefendedPieces, attack, newTable);
}

void ChessAPIService::stepPiece(int from_x, int from_y, int to_x, int to_y) {
  model_->stepPiece(from_x, from_y, to_x, to_y);

  sendTable();
}

void ChessAPIService::newGame() { model_->newGame(); }

ChessField ChessAPIService::getField(int x, int y) {
  return model_->getField(x, y);
}

void ChessAPIService::setHighlighted(int x, int y, bool highlight) {
  model_->setHighlighted(x, y, highlight);
}

void ChessAPIService::switchToQueen(int x, int y, PieceTypes switchTo) {
  model_->switchToQueen(x, y, switchTo);
  sendTable();
}

void ChessAPIService::sendTable() {
  QJsonObject request = {
      {"Function", "deSerializeTable"},
      {"Parameters", QJsonObject{{"Table", model_->serializeTable()}}}};
  sendRequest(request);
}
