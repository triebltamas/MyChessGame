#include "ChessAPIService.h"
#include <QCryptographicHash>

ChessAPIService::ChessAPIService() {}

ChessAPIService::~ChessAPIService() {
  if (requestSocket_ != nullptr && requestSocket_->isOpen()) {
    requestSocket_->close();
    qDebug() << "socket closed";
  }
}

void ChessAPIService::closeSockets() {

  auto closeSocketLambda = [this](QTcpSocket *&socket) {
    if (socket != nullptr) {
      if (socket->isOpen()) {
        socket->close();
      }

      delete socket;
      socket = nullptr;
    }
  };
  auto closeServerLambda = [this](QTcpServer *&server) {
    if (server != nullptr) {
      if (server->isListening())
        server->close();

      delete server;
      server = nullptr;
    }
  };

  if (heartbeatTimer_ != nullptr) {
    heartbeatTimer_->stop();
    delete heartbeatTimer_;
    heartBeated_ = true;
    heartbeatTimer_ = nullptr;
  }

  closeSocketLambda(heartbeatSocket_);
  closeSocketLambda(responseSocket_);
  closeSocketLambda(requestSocket_);

  closeServerLambda(heartbeatServer_);
  closeServerLambda(responseServer_);
}

void ChessAPIService::initSockets() {
  requestSocket_ = new QTcpSocket(this);
  responseServer_ = new QTcpServer(this);
  heartbeatServer_ = new QTcpServer(this);
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
  // STARTING HEARTBEAT SERVER
  counter = 0;
  while (counter < 10000) {
    if (!heartbeatServer_->listen(QHostAddress::Any, heartbeatPort_)) {
      qDebug() << "Heartbeat server could not start";
    } else {
      qDebug() << "Heartbeat server is listening on port " << heartbeatPort_;
      break;
    }
    heartbeatPort_++;
    counter++;
  }

  // CONNECTING TO CHESSSERVER'S REQUEST SERVER
  requestSocket_->connectToHost(serverAddress_, requestPort_);
  if (requestSocket_->waitForConnected(3000)) {
    qDebug() << "Successfully connected to host on IP: " << serverAddress_
             << " and port: " << requestPort_;

    QJsonDocument doc(QJsonObject{
        {"Function", "setPorts"},
        {"Parameters", QJsonObject{{"ResponsePort", responsePort_},
                                   {"HeartbeatPort", heartbeatPort_}}}});
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
        inQueue_ = false;
        opponentsElo_ = parameters["OpponentsElo"].toInt();
        opponentsUsername_ = parameters["OpponentsName"].toString();
        emit startGame(gameSessionID_.second);
      } else if (func == "stepPiece") {
        emit opponentStepped(parameters["Fields"].toObject()["FromX"].toInt(),
                             parameters["Fields"].toObject()["FromY"].toInt(),
                             parameters["Fields"].toObject()["ToX"].toInt(),
                             parameters["Fields"].toObject()["ToY"].toInt());
        if (request.contains("SwitchToQueen")) {
          auto switchField = request["SwitchToQueen"].toObject();
          emit opponentSwitched(
              switchField["X"].toInt(), switchField["Y"].toInt(),
              static_cast<PieceTypes>(switchField["PieceType"].toInt()));
        }
      } else if (func == "gameOverHandled") {
        elo_ = parameters["Elo"].toInt();
        int winner = parameters["Player"].toInt();
        emit gameOver(winner, parameters["Elo"].toInt(),
                      parameters["OpponentDisconnected"].toBool());
        inGame_ = false;
        emit gameEnded(parameters["Elo"].toInt());
      }
    });
  });

  heartbeatTimer_ = new QTimer(this);
  heartbeatTimer_->setInterval(10000);
  connect(heartbeatTimer_, &QTimer::timeout, this, [this]() {
    std::lock_guard<std::mutex> lock(heartbeatMutex_);
    if (!heartBeated_) {
      qDebug() << "Server is not responding...";
      heartbeatTimer_->stop();
      closeSockets();
      userSessionID_ = "";
      userName_ = "";
      emit serverTimedOut();
    } else {
      heartBeated_ = false;
      qDebug() << "Heartbeat recieved";
    }
  });
  heartbeatTimer_->start();
  connect(heartbeatServer_, &QTcpServer::newConnection, this, [this]() {
    heartbeatSocket_ = heartbeatServer_->nextPendingConnection();
    qDebug() << "Successful connection to response server from ip:"
             << heartbeatSocket_->peerAddress().toString()
             << " and port: " << heartbeatPort_;

    connect(heartbeatSocket_, &QTcpSocket::readyRead, this, [this]() {
      auto data = heartbeatSocket_->readAll();
      QString str = QString(data);
      str = str.replace("\n", "");
      QJsonDocument itemDoc = QJsonDocument::fromJson(str.toUtf8());
      QJsonObject request = itemDoc.object();
      QString func = request["Function"].toString();
      if (func == QString("Heartbeat")) {
        qDebug() << "Heartbeat from server";
        std::lock_guard<std::mutex> lock(heartbeatMutex_);
        heartBeated_ = true;
      }
    });
  });
}

void ChessAPIService::loginToServer(QString username, QString password) {
  QCryptographicHash *hash = new QCryptographicHash(QCryptographicHash::Sha1);
  hash->addData(password.toUtf8());
  hash->addData(salt_.toUtf8());

  QJsonObject request = {
      {"Function", "login"},
      {"Parameters",
       QJsonObject{{"UserSessionID", userSessionID_},
                   {"Username", username},
                   {"Password", QString::fromUtf8(hash->result())}}}};

  sendRequest(request);
}
void ChessAPIService::signUpToServer(QString email, QString username,
                                     QString password) {
  QCryptographicHash *hash = new QCryptographicHash(QCryptographicHash::Sha1);
  hash->addData(password.toUtf8());
  hash->addData(salt_.toUtf8());
  QJsonObject request = {
      {"Function", "signUp"},
      {"Parameters",
       QJsonObject{{"UserSessionID", userSessionID_},
                   {"Email", email},
                   {"Username", username},
                   {"Password", QString::fromUtf8(hash->result())}}}};

  sendRequest(request);
}

void ChessAPIService::startQueueing() {
  inQueue_ = true;
  QJsonObject request = {
      {"Function", "startQueueing"},
      {"Parameters", QJsonObject{{"UserSessionID", userSessionID_}}}};

  sendRequest(request);
}

void ChessAPIService::endGameSession(bool logout) {
  QJsonObject request = {
      {"Function", "endGameSession"},
      {"Parameters", QJsonObject{{"UserSessionID", userSessionID_},
                                 {"LogOut", logout},
                                 {"Username", userName_}}}};

  sendRequest(request);
}

void ChessAPIService::setNetworkValues(QString serverAddress, int requestPort,
                                       int responsePort, int heartbeatPort) {
  serverAddress_ = serverAddress;
  requestPort_ = requestPort;
  responsePort_ = responsePort;
  heartbeatPort_ = heartbeatPort;
  closeSockets();
  initSockets();
}

void ChessAPIService::logOut() {
  QJsonObject request = {{"Function", "logOut"},
                         {"Parameters", QJsonObject{{"Username", userName_}}}};

  sendRequest(request);
}

void ChessAPIService::sendRequest(QJsonObject request) {
  if (requestSocket_ != nullptr && !requestSocket_->isWritable()) {
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

void ChessAPIService::stepPiece(int from_x, int from_y, int to_x, int to_y) {
  QJsonObject fieldsJson;

  fieldsJson.insert(QString("FromX"), from_x);
  fieldsJson.insert(QString("FromY"), from_y);
  fieldsJson.insert(QString("ToX"), to_x);
  fieldsJson.insert(QString("ToY"), to_y);
  QJsonObject request = {{"Function", "stepPiece"},
                         {"Parameters", QJsonObject{{"Fields", fieldsJson}}}};

  if (pieceSwitched_) {
    QJsonObject field;

    field.insert(QString("X"), to_x);
    field.insert(QString("Y"), to_y);
    field.insert(QString("PieceType"), static_cast<int>(pieceSwitchedType_));
    request.insert("SwitchToQueen", field);
    pieceSwitched_ = false;
    pieceSwitchedType_ = PieceTypes::VoidType;
  }

  sendRequest(request);
}

void ChessAPIService::onPieceSwitched(PieceTypes switchTo) {
  pieceSwitched_ = true;
  pieceSwitchedType_ = switchTo;
}

bool ChessAPIService::getInGame() { return inGame_; }

bool ChessAPIService::getInQueue() { return inQueue_; }

int ChessAPIService::getElo() { return elo_; }

QString ChessAPIService::getUsername() { return userName_; }

int ChessAPIService::getOpponentsElo() { return opponentsElo_; }

QString ChessAPIService::getOpponentsUsername() { return opponentsUsername_; }

QPair<QString, int> ChessAPIService::getGameSessionID() {
  return gameSessionID_;
}
