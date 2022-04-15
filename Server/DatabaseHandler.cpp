#include "DatabaseHandler.h"

DatabaseHandler::DatabaseHandler(const QString &path) {
  db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName(path);
  if (!db.open()) {
    qDebug() << "COULD NOT OPEN DATABASE";
    return;
  }

  QString queryString = "CREATE TABLE User ("
                        "username	TEXT NOT NULL UNIQUE,"
                        "password	TEXT NOT NULL,"
                        "email	TEXT NOT NULL UNIQUE,"
                        "elo INTEGER NOT NULL DEFAULT 1000,"
                        "PRIMARY KEY('username'))";
  QSqlQuery query;
  if (query.exec(queryString))
    qDebug() << "User Table Created";
  else
    qDebug() << "Database loaded";
}
DatabaseHandler::~DatabaseHandler() { db.close(); }
bool DatabaseHandler::createUser(QString username, QString password,
                                 QString email) {

  QSqlQuery q;
  q.prepare("SELECT count(1) FROM User WHERE username = ? OR email = ?");
  q.addBindValue(username);
  q.addBindValue(email);
  q.exec();
  while (q.next()) {
    auto result = q.value(0).toInt();
    if (result == 1) {
      qDebug() << "User with this username or email already exists";
      return false;
    }
  }

  QSqlQuery query;
  query.prepare("INSERT INTO User ("
                "username,"
                "password,"
                "email,"
                "elo)"
                "VALUES (?,?,?,?);");
  query.addBindValue(username);
  query.addBindValue(password);
  query.addBindValue(email);
  query.addBindValue(1000);

  if (query.exec()) {
    qDebug() << "Successful registration with username '" + username + "'";
    return true;
  }
  return false;
}

bool DatabaseHandler::UserExists(QString username, QString password) {
  QSqlQuery q;
  q.prepare("SELECT count(1) FROM User WHERE username = ? AND password = ?");
  q.addBindValue(username);
  q.addBindValue(password);
  q.exec();
  while (q.next()) {
    auto result = q.value(0).toInt();
    if (result == 1)
      qDebug() << "User with username '" << username << "' exists";
    else
      qDebug() << "User with username '" << username << "' does not exist";
    return result == 1;
  }
  return false;
}

int DatabaseHandler::getElo(QString username) {
  QSqlQuery q;
  q.prepare("SELECT elo FROM User WHERE username = ?");
  q.addBindValue(username);
  q.exec();
  while (q.next()) {
    auto result = q.value(0).toInt();
    return result;
  }
  return -1;
}

bool DatabaseHandler::setElo(QString username, int elo) {
  QSqlQuery q;
  q.prepare("UPDATE User Set elo = ? WHERE username = ?");
  q.addBindValue(elo);
  q.addBindValue(username);
  return q.exec();
}
