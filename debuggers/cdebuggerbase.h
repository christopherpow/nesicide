#ifndef CDEBUGGERBASE_H
#define CDEBUGGERBASE_H

#include <QDockWidget>

#include "emulator_core.h"

class CDebuggerBase : public QDockWidget
{
   Q_OBJECT
public:
   explicit CDebuggerBase(QWidget *parent = 0);
   virtual ~CDebuggerBase();

signals:
   void markProjectDirty(bool dirty);
   void snapTo(QString item);

public slots:

protected:
   NesStateSnapshot m_nesState;
};

#endif // CDEBUGGERBASE_H
