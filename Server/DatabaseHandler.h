#ifndef DATABASEHANDLER_H
#define DATABASEHANDLER_H
#include <QString>
#include <QtSql/QtSql>

class DatabaseHandler {
public:
  DatabaseHandler(const QString &path);
  ~DatabaseHandler();
  bool createUser(QString username, QString password, QString email);
  bool UserExists(QString username, QString password);
  int getElo(QString username);
  void setElo(QString username, int elo);
  bool getOnline(QString username);
  void setOnline(QString username, bool online);
  void setAllUserOnline(bool online);
  QSqlDatabase db;
};

#endif // DATABASEHANDLER_H
