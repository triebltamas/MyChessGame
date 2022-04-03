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
  homePageWidget_ = new HomePageWidget(this);
  ui->centralwidget->layout()->addWidget(homePageWidget_);
  connect(homePageWidget_, &HomePageWidget::localGameButtonClicked, this,
          &ChessMainWindow::onLocalGameClicked);
  connect(homePageWidget_, &HomePageWidget::onlineGameButtonClicked, this,
          &ChessMainWindow::onOnlineGameClicked);
}

ChessMainWindow::~ChessMainWindow() { delete ui; }

void ChessMainWindow::onConnectedToServer() {
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
