#ifndef HOVEREVENTFILTER_H
#define HOVEREVENTFILTER_H
#include <QDebug>
#include <QEvent>
#include <QObject>
class HoverEventFilter : public QObject {
  Q_OBJECT
public:
  HoverEventFilter() {}

protected:
  bool eventFilter(QObject *obj, QEvent *event) override;
};
#endif // HOVEREVENTFILTER_H
