#include "ChessServer.h"
#include <QThread>

ChessServer::ChessServer(QObject *parent) : QObject(parent) {
  model_ = new ChessModel();
  server_ = new QTcpServer(this);
  randomGenerator = new QRandomGenerator();

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
  for (auto session : sessions_) {
    onDisconnected(session.sessionID);
  }
  if (server_->isListening()) {
    qDebug() << "Closing server";
    server_->close();
    server_->deleteLater();
  }

  delete model_;
}

void ChessServer::onGameOver(QString sessionID, int sessionPlayer,
                             int winnerPlayer) {}
void ChessServer::onCheck(QString sessionID, int sessionPlayer) {}
void ChessServer::onDisconnected(QString key) {
  if (!sessionIDs_.contains(key))
    return;

  auto sessionID = sessionIDs_[key];
  auto session = sessions_[sessionID];

  auto p1reqSocket = session.player1RequestSocket;
  auto p1resSocket = session.player1ResponseSocket;
  auto p2reqSocket = session.player2RequestSocket;
  auto p2resSocket = session.player2ResponseSocket;

  if (p1reqSocket != nullptr)
    sessionIDs_.remove(p1reqSocket->peerAddress().toString());

  if (p2reqSocket != nullptr)
    sessionIDs_.remove(p2reqSocket->peerAddress().toString());

  if (p1reqSocket != nullptr && p1reqSocket->isOpen()) {
    p1reqSocket->close();
    p1reqSocket->deleteLater();
  }
  if (p1resSocket != nullptr && p1resSocket->isOpen()) {
    p1resSocket->close();
    p1resSocket->deleteLater();
  }
  if (p2reqSocket != nullptr && p2reqSocket->isOpen()) {
    p2reqSocket->close();
    p2reqSocket->deleteLater();
  }
  if (p2resSocket != nullptr && p2resSocket->isOpen()) {
    p2resSocket->close();
    p2resSocket->deleteLater();
  }

  sessions_.remove(session.sessionID);
  qDebug() << "disconnected players from session:" << sessionID
           << " due to one of the sockets disconnecting";
}
void ChessServer::onNewConnection() {
  QTcpSocket *requestSocket = server_->nextPendingConnection();

  // TODO ezt meg kene csinalni, mert gaz lenne h valaki lecsatlakozik
  //  if (sessionIDs_.contains(requestSocket->peerAddress().toString())) {
  //    return;
  //    // maybe delete socket here
  //  }

  //  connect(requestSocket, &QTcpSocket::disconnected, this,
  //          [=]() { onDisconnected(requestSocket->peerAddress().toString());
  //          });

  connect(requestSocket, &QTcpSocket::readyRead, this, [=]() {
    auto data = requestSocket->readAll();
    qDebug() << data;

    QString str = QString(data);
    str = str.replace("\n", "");
    QJsonDocument itemDoc = QJsonDocument::fromJson(str.toUtf8());
    QJsonObject request = itemDoc.object();
    QString sessionID = request["SessionID"].toString();
    int senderPlayer = request["SessionPlayer"].toInt();

    QString func = request["Function"].toString();

    if (func == "deleteSession") {
      onDisconnected(sessionID);
      return;
    }

    QTcpSocket *socket = nullptr;
    if (senderPlayer == 1)
      socket = sessions_[sessionID].player2ResponseSocket;
    else
      socket = sessions_[sessionID].player1ResponseSocket;

    if (socket == nullptr)
      return;

    socket->write(data);
  });

  QTcpSocket *responseSocket = new QTcpSocket(this);
  responseSocket->connectToHost(requestSocket->peerAddress(), responsePort_);
  if (responseSocket->waitForConnected(3000)) {
    qDebug() << "Successfully connected to host on IP: "
             << requestSocket->peerAddress().toString()
             << " and port: " << responsePort_;
  } else {
    qDebug() << "Failed to connect to host on IP: "
             << requestSocket->peerAddress().toString()
             << " and port: " << responsePort_;
  }

  //  connect(responseSocket, &QTcpSocket::disconnected, this,
  //          [=]() { onDisconnected(requestSocket->peerAddress().toString());
  //          });

  bool hasConnected = false;
  for (auto session : sessions_) {
    if (!session.sessionStarted) {
      session.player2RequestSocket = requestSocket;
      session.player2ResponseSocket = responseSocket;
      session.sessionStarted = true;
      hasConnected = true;

      sessionIDs_.insert(requestSocket->peerAddress().toString(),
                         session.sessionID);

      // send connected to player2
      // TODO ha a lenti mukszik akkor ez is ilyen legyen
      QJsonObject connectedJson;
      connectedJson.insert("Function", "connected");
      connectedJson.insert("Parameters",
                           QJsonObject{{"SessionID", session.sessionID},
                                       {"SessionPlayerNumber", 2}});

      QJsonDocument connectedDoc(connectedJson);
      QByteArray connectedData;
      connectedData.append(QString::fromLatin1(connectedDoc.toJson()));
      session.player2ResponseSocket->write(connectedData);
      session.player2ResponseSocket->waitForBytesWritten(10000);

      QThread::msleep(1000);
      // send start game
      QJsonDocument startGamedoc(QJsonObject{{"Function", "startGame"}});
      QByteArray startGameData;
      startGameData.append(QString::fromLatin1(startGamedoc.toJson()));
      session.player1ResponseSocket->write(startGameData);
      session.player1ResponseSocket->waitForBytesWritten(10000);
      session.player2ResponseSocket->write(startGameData);
      session.player2ResponseSocket->waitForBytesWritten(10000);
      break;
    }
  }
  if (!hasConnected) {
    QString newSessionID = QString::number(randomGenerator->generate());
    GameSession newSession;
    newSession.sessionID = newSessionID;
    newSession.player1RequestSocket = requestSocket;
    newSession.player1ResponseSocket = responseSocket;
    sessions_.insert(newSessionID, newSession);
    sessionIDs_.insert(requestSocket->peerAddress().toString(), newSessionID);

    QJsonObject json;
    json.insert("Function", "connected");
    json.insert("Parameters", QJsonObject{{"SessionID", newSessionID},
                                          {"SessionPlayerNumber", 1}});

    QJsonDocument doc(json);
    QByteArray data;
    data.append(QString::fromLatin1(doc.toJson()));
    newSession.player1ResponseSocket->write(data);
    newSession.player1ResponseSocket->waitForBytesWritten(10000);

    qDebug() << "newSessionID: " << newSessionID;
    // TODO maybe seed random generator here
    randomGenerator->seed();
  }
}
