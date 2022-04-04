#include "ChessMainWindow.h"
#include "ui_ChessMainWindow.h"
#include <QDebug>
#include <QMessageBox>
#include <iostream>

ChessMainWindow::ChessMainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::ChessMainWindow) {
  ui->setupUi(this);
  connect(ui->actionExit, &QAction::triggered, this, &ChessMainWindow::exit);
  connect(ui->actionHomePage, &QAction::triggered, this,
          &ChessMainWindow::homePage);

  ui->menubar->setVisible(false);
  // LOGIN WIDGET
  loginWidget_ = new LoginWidget();

  connect(loginWidget_, &LoginWidget::loggedIn, this,
          &ChessMainWindow::onLoggedIn);

  connect(loginWidget_, &LoginWidget::networkSettingsChanged, this,
          &ChessMainWindow::onNetworkSettingsChanged);

  ui->centralwidget->layout()->addWidget(loginWidget_);

  // CONNECTING TO SERVER
}

ChessMainWindow::~ChessMainWindow() { delete ui; }

void ChessMainWindow::onLoggedIn() {
  // todo leellenorizni h jo e a bejelentkezes
  // ha nem akkor loginWidget_->inCorrectSignIn();

  ui->centralwidget->layout()->removeWidget(loginWidget_);

  if (loginWidget_ != nullptr)
    delete loginWidget_;

  homePageWidget_ = new HomePageWidget(this);
  ui->centralwidget->layout()->addWidget(homePageWidget_);
  connect(homePageWidget_, &HomePageWidget::localGameButtonClicked, this,
          &ChessMainWindow::onLocalGameClicked);
  connect(homePageWidget_, &HomePageWidget::onlineGameButtonClicked, this,
          &ChessMainWindow::onOnlineGameClicked);

  ui->menubar->setVisible(true);
}
void ChessMainWindow::onNetworkSettingsChanged(QString serverAddress,
                                               int requestPort,
                                               int responsePort) {
  if (loginWidget_ == nullptr)
    return;

  serverAddress_ = serverAddress;
  requestPort_ = requestPort;
  responsePort_ = responsePort;

  // todo reconnect to server
  //  loginWidget_->setConnected(success);
  loginWidget_->setConnected(true);
}

void ChessMainWindow::onConnectedToServer() {
  if (loginWidget_ == nullptr)
    return;

  loginWidget_->setConnected(true);

  // todo login screen
}
void ChessMainWindow::onOnlineGameClicked() {
  // todo online
  onlineWidget_ = new OnlineChessWidget();
  ui->centralwidget->layout()->removeWidget(homePageWidget_);
  ui->centralwidget->layout()->addWidget(onlineWidget_);
}
void ChessMainWindow::onLocalGameClicked() {
  // todo local
  localWidget_ = new LocalChessWidget();
  ui->centralwidget->layout()->removeWidget(homePageWidget_);
  ui->centralwidget->layout()->addWidget(localWidget_);
}

void ChessMainWindow::homePage() {
  // todo msgBoxositani
  if (onlineWidget_ != nullptr) {
    ui->centralwidget->layout()->removeWidget(onlineWidget_);
    delete onlineWidget_;
  } else if (localWidget_ != nullptr) {
    ui->centralwidget->layout()->removeWidget(localWidget_);
    delete localWidget_;
  }

  ui->centralwidget->layout()->addWidget(homePageWidget_);
}
void ChessMainWindow::exit() { this->close(); }
