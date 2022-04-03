#ifndef CHESSMAINWINDOW_H
#define CHESSMAINWINDOW_H

#include "ChessAPIService.h"
#include "ChessField.h"
#include "HomePageWidget.h"
#include "LocalChessWidget.h"
#include "OnlineChessWidget.h"
#include "SwitchPawnDialog.h"
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>

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
  void onConnectedToServer();
  void onOnlineGameClicked();
  void onLocalGameClicked();

private:
  void exit();
  void homePage();

  Ui::ChessMainWindow *ui;
  OnlineChessWidget *onlineWidget_ = nullptr;
  LocalChessWidget *localWidget_ = nullptr;
  HomePageWidget *homePageWidget_ = nullptr;
};
#endif // CHESSMAINWINDOW_H
