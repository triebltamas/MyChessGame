#include "ChessAPIService.h"

ChessAPIService::ChessAPIService() : model_(new ChessModel()) {
  requestSocket_ = new QTcpSocket(this);
  responseServer_ = new QTcpServer(this);

  initSockets();

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

void ChessAPIService::closeSockets() {
  if (requestSocket_ != nullptr) {
    if (requestSocket_->isOpen()) {
      requestSocket_->close();
      requestSocket_->disconnectFromHost();
    }

    delete requestSocket_;
  }
  if (responseSocket_ != nullptr) {
    if (responseSocket_->isOpen()) {
      responseSocket_->close();
      responseSocket_->disconnectFromHost();
    }

    delete responseSocket_;
  }
  if (responseServer_ != nullptr) {
    if (responseServer_->isListening())
      responseServer_->close();

    delete responseServer_;
  }
}
void ChessAPIService::initSockets() {
  // STARTING CHESSAPISERVICE'S RESPONSE SERVER
  int counter = 0;
  while (counter < 10000) {
    if (!responseServer_->listen(QHostAddress::Any, responsePort_)) {
      qDebug() << "Response server could not start";
    } else {
      qDebug() << "Response server is listening on port " << responsePort_;
      break;
    }
    responsePort_++;
    counter++;
  }

  // CONNECTING TO CHESSSERVER'S REQUEST SERVER
  requestSocket_->connectToHost(serverAddress_, requestPort_);
  if (requestSocket_->waitForConnected(3000)) {
    qDebug() << "Successfully connected to host on IP: " << serverAddress_
             << " and port: " << requestPort_;

    QJsonDocument doc(
        QJsonObject{{"Function", "responsePort"},
                    {"Parameters", QJsonObject{{"Port", responsePort_}}}});
    QByteArray data;
    data.append(QString::fromLatin1(doc.toJson()));
    requestSocket_->write(data);
    requestSocket_->waitForBytesWritten(1000);

  } else {
    qDebug() << "Failed to connect to host on IP: " << serverAddress_
             << " and port: " << requestPort_;
  }

  connect(responseServer_, &QTcpServer::newConnection, this, [this]() {
    responseSocket_ = responseServer_->nextPendingConnection();
    qDebug() << "Successful connection to response server from ip:"
             << responseSocket_->peerAddress().toString()
             << " and port: " << responsePort_;

    emit connectedToServer();

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
        userSessionID = parameters["UserSessionID"].toString();
        emit connectedToServer();
      } else if (func == "userDisconnected") {
        gameEnded("Opponent disconnected");
      } else if (func == "loginSuccess") {
        emit loginSuccess(parameters["Success"].toBool(),
                          parameters["Message"].toString());
      } else if (func == "createSuccess") {
        emit createSuccess(parameters["Success"].toBool(),
                           parameters["Message"].toString());
      } else if (func == "startGame") {
        gameSessionID.first = parameters["GameSessionID"].toString();
        gameSessionID.second = parameters["SessionPlayerNumber"].toInt();
        inGame = true;
        emit startGame(gameSessionID.second);
      } else if (func == "deSerializeTable") {
        model_->deSerializeTable(parameters["Table"].toObject());
      } else if (func == "deSerializeFields") {
        model_->deSerializeFields(parameters["Fields"].toObject());
      } else if (func == "stepPiece") {
        model_->stepPiece(parameters["Fields"].toObject()["FromX"].toInt(),
                          parameters["Fields"].toObject()["FromY"].toInt(),
                          parameters["Fields"].toObject()["ToX"].toInt(),
                          parameters["Fields"].toObject()["ToY"].toInt());
        emit refreshTable();
      } else if (func == "gameOver") {
        int winner = parameters["Player"].toInt();
        emit gameOver(winner);
        inGame = false;
      } else if (func == "check") {
        emit check();
      }
    });
  });
}

void ChessAPIService::loginToServer(QString username, QString password) {
  QJsonObject request = {
      {"Function", "login"},
      {"Parameters", QJsonObject{{"UserSessionID", userSessionID},
                                 {"Username", username},
                                 {"Password", password}}}};

  sendRequest(request);
}
void ChessAPIService::signUpToServer(QString email, QString username,
                                     QString password) {
  QJsonObject request = {
      {"Function", "signUp"},
      {"Parameters", QJsonObject{{"UserSessionID", userSessionID},
                                 {"Email", email},
                                 {"Username", username},
                                 {"Password", password}}}};

  sendRequest(request);
}

void ChessAPIService::startQueueing() {
  QJsonObject request = {
      {"Function", "startQueueing"},
      {"Parameters", QJsonObject{{"UserSessionID", userSessionID}}}};

  sendRequest(request);
}

void ChessAPIService::endGameSession() {
  QJsonObject request = {
      {"Function", "endGameSession"},
      {"Parameters", QJsonObject{{"UserSessionID", userSessionID}}}};

  sendRequest(request);
}

void ChessAPIService::setNetworkValues(QString serverAddress, int requestPort,
                                       int responsePort) {
  serverAddress_ = serverAddress;
  requestPort_ = requestPort;
  responsePort_ = responsePort;
}

void ChessAPIService::sendRequest(QJsonObject request) {
  if (!requestSocket_->isWritable()) {
    qWarning() << "Socket is not writeable!";
    return;
  }

  if (inGame) {
    request.insert("GameSessionID", gameSessionID.first);
    request.insert("SessionPlayer", gameSessionID.second);
  }

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

  QJsonObject fieldsJson;

  fieldsJson.insert(QString("FromX"), from_x);
  fieldsJson.insert(QString("FromY"), from_y);
  fieldsJson.insert(QString("ToX"), to_x);
  fieldsJson.insert(QString("ToY"), to_y);
  QJsonObject request = {{"Function", "stepPiece"},
                         {"Parameters", QJsonObject{{"Fields", fieldsJson}}}};

  sendRequest(request);
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

int ChessAPIService::getCurrentPlayer() { return model_->getCurrentPLayer(); }
bool ChessAPIService::getInGame() { return inGame; }
