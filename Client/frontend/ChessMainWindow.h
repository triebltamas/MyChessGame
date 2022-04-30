#ifndef CHESSMAINWINDOW_H
#define CHESSMAINWINDOW_H

#include "HomePageWidget.h"
#include "LocalChessWidget.h"
#include "LoginWidget.h"
#include "OnlineChessWidget.h"
#include "SwitchPawnDialog.h"
#include "common/ChessAPIService.h"
#include "common/ChessField.h"
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui {
class ChessMainWindow;
}
QT_END_NAMESPACE

class ChessMainWindow : public QMainWindow {
  Q_OBJECT

public:
  ChessMainWindow(QWidget *parent = nullptr);
  ~ChessMainWindow();

public slots:
  void onConnectedToServer(bool success);
  void onOnlineGameClicked();
  void onLocalGameClicked();
  void onLoginClicked(QString username, QString password);
  void onSignUpClicked(QString email, QString username, QString password);
  void onLoginSuccess(bool success, QString message);
  void onCreateSuccess(bool success, QString message);
  void onNetworkSettingsChanged(QString serverAddress, int requestPort,
                                int responsePort);
  void onGameEnded(QString message, int newElo);
  void onLogoutClicked();

private:
  void exit();
  void homePage();

  Ui::ChessMainWindow *ui;
  std::shared_ptr<ChessAPIService> chessAPIService_ = nullptr;
  OnlineChessWidget *onlineWidget_ = nullptr;
  LocalChessWidget *localWidget_ = nullptr;
  HomePageWidget *homePageWidget_ = nullptr;
  LoginWidget *loginWidget_ = nullptr;
};
#endif // CHESSMAINWINDOW_H
