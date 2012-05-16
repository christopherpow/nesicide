#ifndef APPEVENTFILTER_H
#define APPEVENTFILTER_H

#include <QObject>
#include <QEvent>

class AppEventFilter : public QObject
{
    Q_OBJECT
public:
    explicit AppEventFilter(QObject *parent = 0);

    bool eventFilter(QObject *obj, QEvent *event);

signals:
    void applicationActivationChanged(bool activated);

public slots:


private:
    bool inActivationEvent;
};

#endif // APPEVENTFILTER_H
