#ifndef HOMEPAGEWIDGET_H
#define HOMEPAGEWIDGET_H

#include <QWidget>

namespace Ui {
class HomePageWidget;
}

class HomePageWidget : public QWidget {
  Q_OBJECT

public:
  explicit HomePageWidget();
  ~HomePageWidget();
  void setUsername(QString username);
  void setElo(int elo);

signals:
  void localGameButtonClicked();
  void onlineGameButtonClicked();

private:
  Ui::HomePageWidget *ui;
};

#endif // HOMEPAGEWIDGET_H
