#include "ChessAPIService.h"

ChessAPIService::ChessAPIService() : model_(new ChessModel()) {
  initSockets();

  // CONNECTIONS

  //  connect(model_, &ChessModel::check, this, [this]() {
  //    QJsonObject request = {{"Function", "check"}};
  //    sendRequest(request);
  //  });
  connect(model_, &ChessModel::check, this, &ChessAPIService::check);
  connect(model_, &ChessModel::pawnHasReachedEnemysBase, this,
          [this](int x, int y) {
            if (model_->getCurrentPlayer() != gameSessionID_.second)
              return;

            pieceSwitched_ = true;
            emit pawnHasReachedEnemysBase(x, y);
          });

  connect(model_, &ChessModel::gameOver, this, [this](int Player) {
    if (model_->getCurrentPlayer() == gameSessionID_.second)
      return;

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
  requestSocket_ = new QTcpSocket(this);
  responseServer_ = new QTcpServer(this);
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
    emit connectedToServer(false);
  }

  // INCOMING RESPONSE FROM SERVER
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
        userSessionID_ = parameters["UserSessionID"].toString();
        emit connectedToServer(true);
      } else if (func == "userDisconnected") {
        emit gameEnded("Opponent disconnected", -1);
      } else if (func == "loginSuccess") {
        userName_ = parameters["Username"].toString();
        elo_ = parameters["Elo"].toInt();
        emit loginSuccess(parameters["Success"].toBool(),
                          parameters["Message"].toString());
      } else if (func == "createSuccess") {
        userName_ = parameters["Username"].toString();
        elo_ = parameters["Elo"].toInt();
        emit createSuccess(parameters["Success"].toBool(),
                           parameters["Message"].toString());
      } else if (func == "startGame") {
        gameSessionID_.first = parameters["GameSessionID"].toString();
        gameSessionID_.second = parameters["SessionPlayerNumber"].toInt();
        inGame_ = true;
        emit startGame(gameSessionID_.second);
      } else if (func == "stepPiece") {
        model_->stepPiece(parameters["Fields"].toObject()["FromX"].toInt(),
                          parameters["Fields"].toObject()["FromY"].toInt(),
                          parameters["Fields"].toObject()["ToX"].toInt(),
                          parameters["Fields"].toObject()["ToY"].toInt());
        if (request.contains("SwitchToQueen")) {
          auto switchField = request["SwitchToQueen"].toObject();
          model_->switchToQueen(
              switchField["X"].toInt(), switchField["Y"].toInt(),
              static_cast<PieceTypes>(switchField["PieceType"].toInt()));
        }
        emit refreshTable();
      } else if (func == "gameOverHandled") {
        int winner = parameters["Player"].toInt();
        emit gameOver(winner, parameters["Elo"].toInt());
        inGame_ = false;
        emit gameEnded("", parameters["Elo"].toInt());
      }
    });
  });
}

void ChessAPIService::loginToServer(QString username, QString password) {
  QJsonObject request = {
      {"Function", "login"},
      {"Parameters", QJsonObject{{"UserSessionID", userSessionID_},
                                 {"Username", username},
                                 {"Password", password}}}};

  sendRequest(request);
}
void ChessAPIService::signUpToServer(QString email, QString username,
                                     QString password) {
  QJsonObject request = {
      {"Function", "signUp"},
      {"Parameters", QJsonObject{{"UserSessionID", userSessionID_},
                                 {"Email", email},
                                 {"Username", username},
                                 {"Password", password}}}};

  sendRequest(request);
}

void ChessAPIService::startQueueing() {
  QJsonObject request = {
      {"Function", "startQueueing"},
      {"Parameters", QJsonObject{{"UserSessionID", userSessionID_}}}};

  sendRequest(request);
}

void ChessAPIService::endGameSession() {
  QJsonObject request = {
      {"Function", "endGameSession"},
      {"Parameters", QJsonObject{{"UserSessionID", userSessionID_}}}};

  sendRequest(request);
}

void ChessAPIService::setNetworkValues(QString serverAddress, int requestPort,
                                       int responsePort) {
  serverAddress_ = serverAddress;
  requestPort_ = requestPort;
  responsePort_ = responsePort;

  closeSockets();
  initSockets();
}

void ChessAPIService::logOut() {
  QJsonObject request = {{"Function", "logOut"},
                         {"Parameters", QJsonObject{{"Username", userName_}}}};

  sendRequest(request);
}

void ChessAPIService::sendRequest(QJsonObject request) {
  if (!requestSocket_->isWritable()) {
    qWarning() << "Socket is not writeable!";
    return;
  }

  if (inGame_) {
    request.insert("GameSessionID", gameSessionID_.first);
    request.insert("SessionPlayer", gameSessionID_.second);
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

  if (pieceSwitched_) {
    model_->switchToQueen(to_x, to_y, pieceSwitchedType_);
    QJsonObject field;

    field.insert(QString("X"), to_x);
    field.insert(QString("Y"), to_y);
    field.insert(QString("PieceType"), static_cast<int>(pieceSwitchedType_));
    request.insert("SwitchToQueen", field);
    pieceSwitched_ = false;
    pieceSwitchedType_ = PieceTypes::VoidType;
    refreshTable();
  }

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

  pieceSwitchedType_ = switchTo;
}

int ChessAPIService::getCurrentPlayer() { return model_->getCurrentPlayer(); }
bool ChessAPIService::isMyPiece(int x, int y) {
  return model_->isMyPiece(x, y);
}
bool ChessAPIService::getInGame() { return inGame_; }
int ChessAPIService::getElo() { return elo_; }
QString ChessAPIService::getUsername() { return userName_; }
