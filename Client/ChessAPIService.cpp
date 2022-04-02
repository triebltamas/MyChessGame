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

    connect(responseSocket_, &QTcpSocket::readyRead, this, [this]() {
      auto data = responseSocket_->readAll();
      qDebug() << data;

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
      } else if (func == "deSerializeFields") {
        model_->deSerializeFields(parameters["Fields"].toObject());
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
  });

  // CONNECTIONS

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
  requestSocket_->waitForBytesWritten(10000);
}

QList<QPair<int, int>>
ChessAPIService::possibleSteps(int x, int y, bool includeDefendedPieces,
                               bool attack, bool newTable) {
  return model_->possibleSteps(x, y, includeDefendedPieces, attack, newTable);
}

void ChessAPIService::stepPiece(int from_x, int from_y, int to_x, int to_y) {
  model_->stepPiece(from_x, from_y, to_x, to_y);

  QList<QPair<int, int>> fields;
  fields.append(QPair<int, int>(from_x, from_y));
  fields.append(QPair<int, int>(to_x, to_y));
  sendFields(fields);
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
  QList<QPair<int, int>> fields;
  fields.append(QPair<int, int>(x, y));
  if (x == 0)
    fields.append(QPair<int, int>(x + 1, y));
  else if (x == 7)
    fields.append(QPair<int, int>(x - 1, y));

  sendFields(fields);
}

void ChessAPIService::sendTable() {
  QJsonObject request = {
      {"Function", "deSerializeTable"},
      {"Parameters", QJsonObject{{"Table", model_->serializeTable()}}}};
  sendRequest(request);
}

void ChessAPIService::sendFields(QList<QPair<int, int>> fields) {
  QJsonObject fieldsJson;
  for (auto field : fields) {
    fieldsJson.insert(QString("%1%2").arg(field.first).arg(field.second),
                      model_->serializeField(field.first, field.second));
  }
  QJsonObject request = {{"Function", "deSerializeFields"},
                         {"Parameters", QJsonObject{{"Fields", fieldsJson}}}};

  sendRequest(request);
}
