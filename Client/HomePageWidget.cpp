#include "HomePageWidget.h"
#include "ui_HomePageWidget.h"

HomePageWidget::HomePageWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::HomePageWidget) {
  ui->setupUi(this);
  connect(ui->localGameButton, &QPushButton::clicked, this,
          &HomePageWidget::localGameButtonClicked);

  connect(ui->onlineGameButton, &QPushButton::clicked, this,
          &HomePageWidget::onlineGameButtonClicked);
}

HomePageWidget::~HomePageWidget() { delete ui; }
