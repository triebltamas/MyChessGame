#ifndef HOMEPAGEWIDGET_H
#define HOMEPAGEWIDGET_H

#include <QWidget>

namespace Ui {
class HomePageWidget;
}

class HomePageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HomePageWidget(QWidget *parent = nullptr);
    ~HomePageWidget();

signals:
    void localGameButtonClicked();
    void onlineGameButtonClicked();
private:
    Ui::HomePageWidget *ui;
};

#endif // HOMEPAGEWIDGET_H
