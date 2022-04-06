#include "ChessMainWindow.h"
#include "ui_ChessMainWindow.h"
#include <QDebug>
#include <QMessageBox>
#include <iostream>

ChessMainWindow::ChessMainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::ChessMainWindow),
      chessAPIService_(new ChessAPIService) {
  ui->setupUi(this);
  connect(ui->actionExit, &QAction::triggered, this, &ChessMainWindow::exit);
  connect(ui->actionHomePage, &QAction::triggered, this,
          &ChessMainWindow::homePage);
  connect(chessAPIService_, &ChessAPIService::loginSuccess, this,
          &ChessMainWindow::onLoginSuccess);
  connect(chessAPIService_, &ChessAPIService::createSuccess, this,
          &ChessMainWindow::onCreateSuccess);
  connect(chessAPIService_, &ChessAPIService::gameEnded, this,
          &ChessMainWindow::onGameEnded);

  ui->menubar->setVisible(false);
  // LOGIN WIDGET
  loginWidget_ = new LoginWidget();

  connect(loginWidget_, &LoginWidget::loginClicked, this,
          &ChessMainWindow::onLoginClicked);

  connect(loginWidget_, &LoginWidget::signUpClicked, this,
          &ChessMainWindow::onSignUpClicked);

  connect(loginWidget_, &LoginWidget::networkSettingsChanged, this,
          &ChessMainWindow::onNetworkSettingsChanged);

  ui->centralwidget->layout()->addWidget(loginWidget_);

  // UI
}

ChessMainWindow::~ChessMainWindow() { delete ui; }

void ChessMainWindow::onLoginClicked(QString username, QString password) {
  // todo leellenorizni h jo e a bejelentkezes
  // ha nem akkor loginWidget_->inCorrectSignIn();

  chessAPIService_->loginToServer(username, password);
}
void ChessMainWindow::onSignUpClicked(QString email, QString username,
                                      QString password) {
  // todo leellenorizni h jo e a bejelentkezes
  // ha nem akkor loginWidget_->inCorrectSignIn();

  chessAPIService_->signUpToServer(email, username, password);
}
void ChessMainWindow::onNetworkSettingsChanged(QString serverAddress,
                                               int requestPort,
                                               int responsePort) {
  if (loginWidget_ == nullptr)
    return;

  chessAPIService_->setNetworkValues(serverAddress, requestPort, responsePort);

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
  onlineWidget_ = new OnlineChessWidget(chessAPIService_);
  ui->centralwidget->layout()->removeWidget(homePageWidget_);
  ui->centralwidget->layout()->addWidget(onlineWidget_);

  if (homePageWidget_ != nullptr)
    delete homePageWidget_;
}
void ChessMainWindow::onLocalGameClicked() {
  // todo local
  localWidget_ = new LocalChessWidget();
  ui->centralwidget->layout()->removeWidget(homePageWidget_);
  ui->centralwidget->layout()->addWidget(localWidget_);

  if (homePageWidget_ != nullptr)
    delete homePageWidget_;
}

void ChessMainWindow::homePage() {
  // todo msgBoxositani
  if (chessAPIService_->getInGame())
    chessAPIService_->endGameSession();

  if (onlineWidget_ != nullptr) {
    ui->centralwidget->layout()->removeWidget(onlineWidget_);
    delete onlineWidget_;
  } else if (localWidget_ != nullptr) {
    ui->centralwidget->layout()->removeWidget(localWidget_);
    delete localWidget_;
  }

  homePageWidget_ = new HomePageWidget();
  ui->centralwidget->layout()->addWidget(homePageWidget_);
}
void ChessMainWindow::exit() { this->close(); }
void ChessMainWindow::connectToServer() {}
void ChessMainWindow::onGameEnded(QString message) {
  // todo msg boxba kirakni a messaget
  homePage();
}
void ChessMainWindow::onLoginSuccess(bool success, QString message) {
  if (!success) {
    // todo set loginwarningmsg
    loginWidget_->inCorrectSignIn();
    return;
  }

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

void ChessMainWindow::onCreateSuccess(bool success, QString message) {
  if (!success) {
    // todo set loginwarningmsg
    loginWidget_->inCorrectSignUp();
    return;
  }

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
