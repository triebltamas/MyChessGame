#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>

namespace Ui {
class LoginWidget;
}

class LoginWidget : public QWidget {
  Q_OBJECT

public:
  explicit LoginWidget(QWidget *parent = nullptr);
  ~LoginWidget();

  void setConnected(bool connected);
  void inCorrectSignIn();
  void inCorrectSignUp();
signals:
  void loggedIn();
  void networkSettingsChanged(QString serverAddress, int requestPort,
                              int responsePort);

private slots:
  void onSignInClicked();
  void onNewSignUp();
  void onNetworkSettingsChanged();

private:
  void updateWarningLabels(bool visible);
  Ui::LoginWidget *ui;
  bool isConnected_ = false;
};

#endif // LOGINWIDGET_H
