#include "HoverEventFilter.h"

bool HoverEventFilter::eventFilter(QObject *obj, QEvent *event) {
  Q_UNUSED(obj)
  if (event->type() == QEvent::HoverEnter) {
    return true;
  } else if (event->type() == QEvent::Resize) {
    //    qDebug() << "asd";
    // todo itt valahogy az ikonokat resizolni megfeleloen
  }

  return false;
}
