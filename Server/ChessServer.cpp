#include "ChessServer.h"
#include <QThread>

ChessServer::ChessServer(QObject *parent) : QObject(parent) {
  initServer();
  randomGenerator_ = new QRandomGenerator(1234);
  databaseHandler_ = new DatabaseHandler(dbPath_);
  databaseHandler_->setAllUserOffline();

  server_ = new QTcpServer(this);
  connect(server_, &QTcpServer::newConnection, this,
          &ChessServer::onNewConnection);

  if (!server_->listen(QHostAddress::Any, requestPort_)) {
    qDebug() << "Server could not start";
    exit(EXIT_FAILURE);
  } else {
    qDebug() << "Server is listening on port " << requestPort_;
  }
}

ChessServer::~ChessServer() {
  if (server_->isListening()) {
    qDebug() << "Closing server";
    server_->close();
    server_->deleteLater();
  }
  if (randomGenerator_ != nullptr)
    delete randomGenerator_;

  if (databaseHandler_ != nullptr)
    delete databaseHandler_;
}

void ChessServer::initServer() {
  QString absolutePath =
      QDir(QCoreApplication::applicationDirPath()).filePath("server.ini");

  if (QFile(absolutePath).exists()) {
    QSettings serverSettings(absolutePath, QSettings::IniFormat);
    serverSettings.beginGroup("GeneralSettings");
    dbPath_ = serverSettings.value("path").toString();
    auto port = serverSettings.value("port");
    requestPort_ = port.toInt();
    serverSettings.endGroup();

    if (dbPath_ == "" || port.toString() == "") {
      qWarning() << "The server.ini file is invalid. Example of a correct "
                    "file:\n---------------------------------------\n\n["
                    "GeneralSettings]\npath=MY_PATH/"
                    "DATABASE_NAME.sqlite\nport=1337\n\n---"
                    "------------------------------------\nPath for file: "
                 << absolutePath;
      exit(EXIT_FAILURE);
    }

  } else {
    qWarning() << "The server.ini file does not exist, please create it here: "
               << QCoreApplication::applicationDirPath();
    qWarning() << "Example of a correct "
                  "file:\n---------------------------------------\n\n["
                  "GeneralSettings]\npath=MY_PATH/"
                  "DATABASE_NAME.sqlite\nport=1337\n\n---"
                  "------------------------------------";
    exit(EXIT_FAILURE);
  }
}

void ChessServer::onGameOver(QString sessionID, int winnerPlayer,
                             bool opponentDisconnected) {
  if (!gameSessions_.contains(sessionID))
    return;

  auto session = gameSessions_[sessionID];

  auto username1 = gameSessions_[sessionID].player1.username;
  auto username2 = gameSessions_[sessionID].player2.username;

  int elo1 = databaseHandler_->getElo(username1);
  int elo2 = databaseHandler_->getElo(username2);

  auto ratings = getNewEloRating(elo1, elo2, winnerPlayer);

  databaseHandler_->setElo(username1, ratings.first);
  databaseHandler_->setElo(username2, ratings.second);

  QJsonObject json1 = {{"Function", "gameOverHandled"},
                       {"Parameters", QJsonObject{{"Player", winnerPlayer},
                                                  {"Elo", ratings.first},
                                                  {"OpponentDisconnected",
                                                   opponentDisconnected}}}};
  QJsonDocument doc1(json1);
  QByteArray data1;
  data1.append(QString::fromLatin1(doc1.toJson()));

  QJsonObject json2 = {{"Function", "gameOverHandled"},
                       {"Parameters", QJsonObject{{"Player", winnerPlayer},
                                                  {"Elo", ratings.second},
                                                  {"OpponentDisconnected",
                                                   opponentDisconnected}}}};
  QJsonDocument doc2(json2);
  QByteArray data2;
  data2.append(QString::fromLatin1(doc2.toJson()));

  writeToClient(session.player1.responseSocket, data1);
  writeToClient(session.player2.responseSocket, data2);
}
void ChessServer::onCheck(QString sessionID, int sessionPlayer) {}

void ChessServer::onDisconnected(QString key) {
  if (!userSessions_.contains(key))
    return;
  auto requestSocket = userSessions_[key].requestSocket;
  auto responseSocket = userSessions_[key].responseSocket;

  if (requestSocket != nullptr && requestSocket->isOpen()) {
    requestSocket->close();
    requestSocket->deleteLater();
  }
  if (responseSocket != nullptr && responseSocket->isOpen()) {
    responseSocket->close();
    responseSocket->deleteLater();
  }
}

void ChessServer::onNewConnection() {
  QTcpSocket *requestSocket = server_->nextPendingConnection();

  connect(requestSocket, &QTcpSocket::readyRead, this, [=]() {
    auto data = requestSocket->readAll();
    qDebug() << data;

    QString str = QString(data);
    str = str.replace("\n", "");
    QJsonDocument itemDoc = QJsonDocument::fromJson(str.toUtf8());
    QJsonObject request = itemDoc.object();
    QString sessionID = request["GameSessionID"].toString();
    int senderPlayer = request["SessionPlayer"].toInt();

    QString func = request["Function"].toString();
    QJsonObject parameters = request["Parameters"].toObject();

    if (func == "responsePort") {
      onResponseSockectAvailable(requestSocket->peerAddress(),
                                 parameters["Port"].toInt(), requestSocket);
      return;
    } else if (func == "login") {
      loginUser(parameters["UserSessionID"].toString(),
                parameters["Username"].toString(),
                parameters["Password"].toString());
      return;
    } else if (func == "signUp") {
      createUser(parameters["UserSessionID"].toString(),
                 parameters["Email"].toString(),
                 parameters["Username"].toString(),
                 parameters["Password"].toString());
      return;
    } else if (func == "startQueueing") {
      onStartQueueing(parameters["UserSessionID"].toString());
      return;
    } else if (func == "endGameSession") {
      if (parameters["LogOut"].toBool())
        databaseHandler_->setOnline(parameters["Username"].toString(), false);

      endGameSession(parameters["UserSessionID"].toString());

      return;
    } else if (func == "gameOver") {
      onGameOver(sessionID, parameters["Player"].toInt());
      return;
    } else if (func == "logOut") {
      databaseHandler_->setOnline(parameters["Username"].toString(), false);
      return;
    }

    QTcpSocket *socket = nullptr;
    if (senderPlayer == 1)
      socket = gameSessions_[sessionID].player2.responseSocket;
    else
      socket = gameSessions_[sessionID].player1.responseSocket;

    if (socket == nullptr)
      return;

    socket->write(data);
  });
}

void ChessServer::onResponseSockectAvailable(QHostAddress address,
                                             int responsePort,
                                             QTcpSocket *requestSocket) {
  QTcpSocket *responseSocket = new QTcpSocket(this);
  responseSocket->connectToHost(address, responsePort);
  if (responseSocket->waitForConnected(3000)) {
    qDebug() << "Successfully connected to host on IP: " << address.toString()
             << " and port: " << responsePort;

    QString newSessionID;

    int counter = 0;
    while (counter < 1000) {
      newSessionID = QString::number(randomGenerator_->generate());
      if (!userSessions_.contains(newSessionID))
        break;

      counter++;
    }

    UserSession session;
    session.sessionID = newSessionID;
    session.requestSocket = requestSocket;
    session.responseSocket = responseSocket;

    userSessions_[newSessionID] = session;

    QJsonDocument doc(QJsonObject{
        {"Function", "connected"},
        {"Parameters", QJsonObject{{"UserSessionID", newSessionID}}}});
    QByteArray data;
    data.append(QString::fromLatin1(doc.toJson()));
    writeToClient(responseSocket, data);

    connect(responseSocket, &QTcpSocket::disconnected, this, [=]() {
      if (userSessions_[newSessionID].inGame)
        endGameSession(newSessionID);

      onDisconnected(newSessionID);
    });

    connect(requestSocket, &QTcpSocket::disconnected, this, [=]() {
      if (userSessions_[newSessionID].inGame)
        endGameSession(newSessionID);

      onDisconnected(newSessionID);
    });

  } else {
    qDebug() << "Failed to connect to host on IP: " << address.toString()
             << " and port: " << responsePort;
  }
}

void ChessServer::onStartQueueing(QString userSessionID) {
  bool hasConnected = false;
  for (auto session : gameSessions_) {
    if (!session.sessionStarted) {
      session.player2 = userSessions_[userSessionID];
      session.sessionStarted = true;
      session.player2.inGame = true;

      hasConnected = true;
      // send start to both clients
      QJsonObject json1;
      json1.insert("Function", "startGame");
      json1.insert(
          "Parameters",
          QJsonObject{{"GameSessionID", session.sessionID},
                      {"SessionPlayerNumber", 1},
                      {"OpponentsName", session.player2.username},
                      {"OpponentsElo",
                       databaseHandler_->getElo(session.player2.username)}});

      QJsonDocument doc1(json1);
      QByteArray data1;
      data1.append(QString::fromLatin1(doc1.toJson()));
      writeToClient(session.player1.responseSocket, data1);

      QJsonObject json2;
      json2.insert("Function", "startGame");
      json2.insert(
          "Parameters",
          QJsonObject{{"GameSessionID", session.sessionID},
                      {"SessionPlayerNumber", 2},
                      {"OpponentsName", session.player1.username},
                      {"OpponentsElo",
                       databaseHandler_->getElo(session.player1.username)}});

      QJsonDocument doc2(json2);
      QByteArray data2;
      data2.append(QString::fromLatin1(doc2.toJson()));
      writeToClient(session.player2.responseSocket, data2);

      gameSessions_[session.sessionID] = session;
      break;
    }
  }
  if (!hasConnected) {
    QString newGameSessionID;

    int counter = 0;
    while (counter < 1000) {
      newGameSessionID = QString::number(randomGenerator_->generate());
      if (!gameSessions_.contains(newGameSessionID))
        break;

      counter++;
    }
    GameSession newSession;
    newSession.sessionID = newGameSessionID;
    newSession.player1 = userSessions_[userSessionID];
    newSession.player1.inGame = true;
    gameSessions_[newGameSessionID] = newSession;

    qDebug() << "newGameSessionID: " << newGameSessionID;
  }
}

void ChessServer::endGameSession(QString userSessionID) {
  QString gameSessionID = "";
  for (auto session : gameSessions_) {
    if (session.player1.sessionID == userSessionID) {
      gameSessionID = session.sessionID;
      if (!session.sessionStarted)
        break;

      // player 2 wins
      onGameOver(gameSessionID, 2, true);

      break;

    } else if (session.player2.sessionID == userSessionID) {
      gameSessionID = session.sessionID;
      if (!session.sessionStarted)
        break;

      // player 1 wins
      onGameOver(gameSessionID, 1, true);

      break;
    }
  }
  gameSessions_.remove(gameSessionID);
}
void ChessServer::writeToClient(QTcpSocket *socket, QByteArray data) {
  if (socket == nullptr)
    return;

  socket->write(data);
  socket->waitForBytesWritten(1000);
}
void ChessServer::loginUser(QString userSessionID, QString username,
                            QString password) {
  bool exists = databaseHandler_->UserExists(username, password);
  bool online = true;
  if (exists) {
    online = databaseHandler_->getOnline(username);

    if (!online) {
      userSessions_[userSessionID].username = username;
      databaseHandler_->setOnline(username, true);
    }
  }
  bool success = exists && !online;

  QJsonObject json;
  json.insert("Function", "loginSuccess");
  json.insert("Parameters",
              QJsonObject{{"Success", success},
                          {"Message", ""},
                          {"Username", username},
                          {"Elo", databaseHandler_->getElo(username)}});
  QJsonDocument doc(json);
  QByteArray data;
  data.append(QString::fromLatin1(doc.toJson()));
  writeToClient(userSessions_[userSessionID].responseSocket, data);
}

void ChessServer::createUser(QString userSessionID, QString email,
                             QString username, QString password) {
  bool success = databaseHandler_->createUser(username, password, email);
  if (success) {
    userSessions_[userSessionID].username = username;
    databaseHandler_->setOnline(username, true);
  }

  // itt kell a dbvel megnezni h lehet e regisztralni
  QJsonObject json;
  json.insert("Function", "createSuccess");
  json.insert("Parameters",
              QJsonObject{{"Success", success},
                          {"Message", ""},
                          {"Username", username},
                          {"Elo", databaseHandler_->getElo(username)}});
  QJsonDocument doc(json);
  QByteArray data;
  data.append(QString::fromLatin1(doc.toJson()));
  writeToClient(userSessions_[userSessionID].responseSocket, data);
}

float ChessServer::getProbability(int rating1, int rating2) {
  return 1.0 * 1.0 / (1 + 1.0 * pow(10, 1.0 * (rating1 - rating2) / 400));
}

QPair<int, int> ChessServer::getNewEloRating(float player1, float player2,
                                             int winnerPlayer) {
  float P2 = getProbability(player1, player2);

  float P1 = getProbability(player2, player1);

  if (winnerPlayer == 0) {
    player1 = player1 + constant_ * (0.5 - P1);
    player2 = player2 + constant_ * (0.5 - P2);
  } else if (winnerPlayer == 1) {
    player1 = player1 + constant_ * (1 - P1);
    player2 = player2 + constant_ * (0 - P2);
  } else if (winnerPlayer == 2) {
    player1 = player1 + constant_ * (0 - P1);
    player2 = player2 + constant_ * (1 - P2);
  }
  return QPair<int, int>(player1, player2);
}
