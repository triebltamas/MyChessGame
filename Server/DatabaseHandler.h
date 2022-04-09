#ifndef DATABASEHANDLER_H
#define DATABASEHANDLER_H
#include <QString>
#include <QtSql/QtSql>

class DatabaseHandler {
public:
  DatabaseHandler();
  ~DatabaseHandler();
  bool createUser(QString username, QString password, QString email);
  bool UserExists(QString username, QString password);
  int getElo(QString username);
  bool setElo(QString username, int elo);
  QSqlDatabase db;
};

#endif // DATABASEHANDLER_H
