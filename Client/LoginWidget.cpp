#include "LoginWidget.h"
#include "ui_LoginWidget.h"

LoginWidget::LoginWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::LoginWidget) {
  ui->setupUi(this);

  // INIT_UI
  ui->passwordEdit->setEchoMode(QLineEdit::EchoMode::Password);
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
  // todo db check
  emit loggedIn();
}

void LoginWidget::onNewSignUp() {
  // todo db create user
  emit loggedIn();
}

void LoginWidget::onNetworkSettingsChanged() {
  emit networkSettingsChanged(ui->serverAddressEdit->text(),
                              ui->requestPortSpinBox->value(),
                              ui->responsePortSpinBox->value());

  ui->signInButton_3->setEnabled(false);
  ui->networkWaitLabel->setVisible(true);
  // todo wait if able to connect
}

void LoginWidget::setConnected(bool connected) {
  isConnected_ = connected;
  ui->signInButton_3->setEnabled(connected);
  ui->networkWaitLabel->setVisible(!connected);
  if (!connected) {
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