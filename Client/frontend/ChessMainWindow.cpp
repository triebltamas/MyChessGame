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
  connect(ui->actionLogOut, &QAction::triggered, this,
          &ChessMainWindow::onLogoutClicked);

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

  // API

  chessAPIService_ = std::shared_ptr<ChessAPIService>(new ChessAPIService);
  connect(chessAPIService_.get(), &ChessAPIService::loginSuccess, this,
          &ChessMainWindow::onLoginSuccess);
  connect(chessAPIService_.get(), &ChessAPIService::createSuccess, this,
          &ChessMainWindow::onCreateSuccess);
  connect(chessAPIService_.get(), &ChessAPIService::gameEnded, this,
          &ChessMainWindow::onGameEnded);
  connect(chessAPIService_.get(), &ChessAPIService::serverTimedOut, this,
          &ChessMainWindow::onServerTimedOut);
  connect(chessAPIService_.get(), &ChessAPIService::connectedToServer, this,
          &ChessMainWindow::onConnectedToServer, Qt::DirectConnection);
  chessAPIService_->initSockets();
}

ChessMainWindow::~ChessMainWindow() {
  if (chessAPIService_->getInGame() || chessAPIService_->getInQueue())
    chessAPIService_->endGameSession(true);
  else
    chessAPIService_->logOut();

  chessAPIService_->closeSockets();
  delete ui;
}

void ChessMainWindow::onLogoutClicked() {
  if (loginWidget_ != nullptr)
    return;

  if (chessAPIService_->getInGame() || chessAPIService_->getInQueue())
    chessAPIService_->endGameSession(true);
  else
    chessAPIService_->logOut();

  ui->centralwidget->layout()->removeWidget(loginWidget_);
  ui->centralwidget->layout()->removeWidget(onlineWidget_);
  ui->centralwidget->layout()->removeWidget(localWidget_);
  ui->centralwidget->layout()->removeWidget(homePageWidget_);
  if (onlineWidget_ != nullptr) {
    delete onlineWidget_;
    onlineWidget_ = nullptr;
  } else if (localWidget_ != nullptr) {
    delete localWidget_;
    localWidget_ = nullptr;
  } else if (homePageWidget_ != nullptr) {
    delete homePageWidget_;
    homePageWidget_ = nullptr;
  }

  loginWidget_ = new LoginWidget();
  ui->centralwidget->layout()->addWidget(loginWidget_);
  connect(loginWidget_, &LoginWidget::loginClicked, this,
          &ChessMainWindow::onLoginClicked);

  connect(loginWidget_, &LoginWidget::signUpClicked, this,
          &ChessMainWindow::onSignUpClicked);

  connect(loginWidget_, &LoginWidget::networkSettingsChanged, this,
          &ChessMainWindow::onNetworkSettingsChanged);

  ui->menubar->setVisible(false);
}

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
                                               int responsePort,
                                               int heartbeatPort) {
  if (loginWidget_ == nullptr)
    return;

  chessAPIService_->setNetworkValues(serverAddress, requestPort, responsePort,
                                     heartbeatPort);
}

void ChessMainWindow::onConnectedToServer(bool connected) {
  if (loginWidget_ == nullptr)
    return;

  loginWidget_->setConnected(connected);

  // todo login screen
}

void ChessMainWindow::onServerTimedOut() {
  ui->centralwidget->layout()->removeWidget(loginWidget_);
  ui->centralwidget->layout()->removeWidget(onlineWidget_);
  ui->centralwidget->layout()->removeWidget(localWidget_);
  ui->centralwidget->layout()->removeWidget(homePageWidget_);
  if (homePageWidget_ != nullptr) {
    delete homePageWidget_;
    homePageWidget_ = nullptr;
  } else if (loginWidget_ != nullptr) {
    delete loginWidget_;
    loginWidget_ = nullptr;
  } else if (onlineWidget_ != nullptr) {
    delete onlineWidget_;
    onlineWidget_ = nullptr;
  } else if (localWidget_ != nullptr) {
    delete localWidget_;
    localWidget_ = nullptr;
  }

  loginWidget_ = new LoginWidget();
  ui->centralwidget->layout()->addWidget(loginWidget_);
  connect(loginWidget_, &LoginWidget::loginClicked, this,
          &ChessMainWindow::onLoginClicked);

  connect(loginWidget_, &LoginWidget::signUpClicked, this,
          &ChessMainWindow::onSignUpClicked);

  connect(loginWidget_, &LoginWidget::networkSettingsChanged, this,
          &ChessMainWindow::onNetworkSettingsChanged);

  ui->menubar->setVisible(false);
  loginWidget_->setConnected(false);
}

void ChessMainWindow::onOnlineGameClicked() {
  // todo online
  onlineWidget_ = new OnlineChessWidget(chessAPIService_);
  ui->centralwidget->layout()->removeWidget(homePageWidget_);
  ui->centralwidget->layout()->addWidget(onlineWidget_);

  if (homePageWidget_ != nullptr) {
    delete homePageWidget_;
    homePageWidget_ = nullptr;
  }
}
void ChessMainWindow::onLocalGameClicked() {
  // todo local
  localWidget_ = new LocalChessWidget();
  ui->centralwidget->layout()->removeWidget(homePageWidget_);
  ui->centralwidget->layout()->addWidget(localWidget_);

  if (homePageWidget_ != nullptr) {
    delete homePageWidget_;
    homePageWidget_ = nullptr;
  }
}

void ChessMainWindow::homePage() {
  if (homePageWidget_ != nullptr)
    return;

  // todo msgBoxositani
  if (chessAPIService_->getInGame() || chessAPIService_->getInQueue())
    chessAPIService_->endGameSession(false);

  ui->centralwidget->layout()->removeWidget(loginWidget_);
  ui->centralwidget->layout()->removeWidget(onlineWidget_);
  ui->centralwidget->layout()->removeWidget(localWidget_);
  ui->centralwidget->layout()->removeWidget(homePageWidget_);
  if (loginWidget_ != nullptr) {
    delete loginWidget_;
    loginWidget_ = nullptr;
  } else if (onlineWidget_ != nullptr) {
    delete onlineWidget_;
    onlineWidget_ = nullptr;
  } else if (localWidget_ != nullptr) {
    delete localWidget_;
    localWidget_ = nullptr;
  }

  homePageWidget_ = new HomePageWidget();
  ui->centralwidget->layout()->addWidget(homePageWidget_);
  connect(homePageWidget_, &HomePageWidget::localGameButtonClicked, this,
          &ChessMainWindow::onLocalGameClicked);
  connect(homePageWidget_, &HomePageWidget::onlineGameButtonClicked, this,
          &ChessMainWindow::onOnlineGameClicked);

  homePageWidget_->setUsername(chessAPIService_->getUsername());
  homePageWidget_->setElo(chessAPIService_->getElo());

  ui->menubar->setVisible(true);
}
void ChessMainWindow::exit() { this->close(); }
void ChessMainWindow::onGameEnded(int newElo) {
  if (onlineWidget_ == nullptr) {
    QMessageBox::information(
        this, tr("Game over"),
        QString("You surrendered!\nNew elo: %1").arg(newElo));
  }

  homePage();

  if (newElo != -1 && homePageWidget_ != nullptr)
    homePageWidget_->setElo(newElo);
}
void ChessMainWindow::onLoginSuccess(bool success, QString message) {
  if (!success) {
    // todo set loginwarningmsg
    loginWidget_->inCorrectSignIn();
    return;
  }

  homePage();
}

void ChessMainWindow::onCreateSuccess(bool success, QString message) {
  if (!success) {
    // todo set loginwarningmsg
    loginWidget_->inCorrectSignUp();
    return;
  }

  homePage();
}
