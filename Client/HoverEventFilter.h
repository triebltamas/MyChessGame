#ifndef HOVEREVENTFILTER_H
#define HOVEREVENTFILTER_H
#include <QObject>
#include <QEvent>
class HoverEventFilter : public QObject
{
    Q_OBJECT
public:
    HoverEventFilter(){}
protected:
    bool eventFilter(QObject *obj, QEvent *event) override
    {
        Q_UNUSED(obj)
        if (event->type() == QEvent::HoverEnter)
        {
            return true;
        }

        return false;
    }
};
#endif // HOVEREVENTFILTER_H
