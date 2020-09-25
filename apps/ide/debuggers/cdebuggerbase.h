#ifndef CDEBUGGERBASE_H
#define CDEBUGGERBASE_H

#include <QDockWidget>

#include "nes_emulator_core.h"

class CDebuggerBase : public QDockWidget
{
   Q_OBJECT
public:
   explicit CDebuggerBase(QWidget *parent = 0);
   virtual ~CDebuggerBase();

signals:
   void markProjectDirty(bool dirty);
   void snapTo(QString item);
   void setStatusBarMessage(QString message);
   void addStatusBarWidget(QWidget* widget);
   void removeStatusBarWidget(QWidget* widget);

public slots:
   void snapToHandler(QString item) {};
   void applyEnvironmentSettings() {};

protected:
   NesStateSnapshot m_nesState;
};

#endif // CDEBUGGERBASE_H
