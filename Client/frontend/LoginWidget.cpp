#include "LoginWidget.h"
#include "ui_LoginWidget.h"

LoginWidget::LoginWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::LoginWidget) {
  ui->setupUi(this);

  // INIT_UI
  ui->passwordEdit->setEchoMode(QLineEdit::EchoMode::Password);
  ui->passwordEdit_2->setEchoMode(QLineEdit::EchoMode::Password);
  ui->stackedWidget->setCurrentWidget(ui->signInPage);
  updateWarningLabels(false);

  // CONNECTIONS
  // Sign in page
  connect(ui->signInButton, &QPushButton::clicked, this,
          &LoginWidget::onSignInClicked);

  connect(ui->passwordEdit, &QLineEdit::returnPressed, this,
          &LoginWidget::onSignInClicked);

  connect(ui->signUpButton, &QPushButton::clicked, this, [this]() {
    ui->stackedWidget->setCurrentWidget(ui->signUpPage);
    updateWarningLabels(false);
  });

  connect(ui->networkButton, &QPushButton::clicked, this, [this]() {
    ui->stackedWidget->setCurrentWidget(ui->networkPage);
    updateWarningLabels(false);
  });

  // Sign up page
  connect(ui->signInButton_2, &QPushButton::clicked, this, [this]() {
    ui->stackedWidget->setCurrentWidget(ui->signInPage);
    updateWarningLabels(false);
  });

  connect(ui->signUpButton_2, &QPushButton::clicked, this,
          &LoginWidget::onNewSignUp);

  // Network settings page
  connect(ui->configureButton, &QPushButton::clicked, this,
          &LoginWidget::onNetworkSettingsChanged);

  connect(ui->signInButton_3, &QPushButton::clicked, this, [this]() {
    ui->stackedWidget->setCurrentWidget(ui->signInPage);
    updateWarningLabels(false);
  });
}

LoginWidget::~LoginWidget() { delete ui; }

void LoginWidget::onSignInClicked() {
  emit loginClicked(ui->usernameEdit->text(), ui->passwordEdit->text());
}

void LoginWidget::onNewSignUp() {
  if (ui->passwordEdit_2->text() == "" || ui->usernameEdit_2->text() == "" ||
      !isEmailValid(ui->emailEdit->text())) {
    inCorrectSignUp();
    return;
  }

  emit signUpClicked(ui->emailEdit->text(), ui->usernameEdit_2->text(),
                     ui->passwordEdit_2->text());
}

void LoginWidget::onNetworkSettingsChanged() {
  ui->signInButton_3->setEnabled(false);
  ui->networkWaitLabel->setVisible(true);
  ui->networkWarningLabel->setVisible(false);
  this->repaint();
  emit networkSettingsChanged(
      ui->serverAddressEdit->text(), ui->requestPortSpinBox->value(),
      ui->responsePortSpinBox->value(), ui->heartbeatPortSpinBox->value());
}

void LoginWidget::setConnected(bool connected) {
  isConnected_ = connected;
  ui->signInButton_3->setEnabled(connected);
  ui->networkWaitLabel->setVisible(false);
  if (!connected) {
    ui->networkWarningLabel->setVisible(true);
    ui->stackedWidget->setCurrentWidget(ui->networkPage);
  }
}

void LoginWidget::inCorrectSignIn() {
  ui->signInWarningLabel->setVisible(true);
}

void LoginWidget::inCorrectSignUp() {
  ui->signUpWarningLabel->setVisible(true);
}

void LoginWidget::updateWarningLabels(bool visible) {
  ui->networkWarningLabel->setVisible(visible);
  ui->signInWarningLabel->setVisible(visible);
  ui->signUpWarningLabel->setVisible(visible);
  ui->networkWaitLabel->setVisible(visible);
}

bool LoginWidget::isEmailValid(QString email) {
  if (email == "" || !email.contains('@'))
    return false;

  return true;
}
