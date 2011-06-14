#ifndef CDEBUGGERBASE_H
#define CDEBUGGERBASE_H

#include <QDockWidget>

class CDebuggerBase : public QDockWidget
{
   Q_OBJECT
public:
   explicit CDebuggerBase(QWidget *parent = 0);

signals:
   void markProjectDirty(bool dirty);

public slots:

};

#endif // CDEBUGGERBASE_H
