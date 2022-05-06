#include "HomePageWidget.h"
#include "ui_HomePageWidget.h"

HomePageWidget::HomePageWidget() : ui(new Ui::HomePageWidget) {
  ui->setupUi(this);
  connect(ui->localGameButton, &QPushButton::clicked, this,
          &HomePageWidget::localGameButtonClicked);

  connect(ui->onlineGameButton, &QPushButton::clicked, this,
          &HomePageWidget::onlineGameButtonClicked);

  ui->localGameButton->setFocusPolicy(Qt::NoFocus);
  ui->onlineGameButton->setFocusPolicy(Qt::NoFocus);
}

HomePageWidget::~HomePageWidget() { delete ui; }

void HomePageWidget::setUsername(QString username) {
  ui->usernameLabel->setText("Username: " + username);
}
void HomePageWidget::setElo(int elo) {
  ui->eloLabel->setText(QString("Elo: %1").arg(elo));
}
