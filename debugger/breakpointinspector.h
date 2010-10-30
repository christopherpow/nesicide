#ifndef BREAKPOINTINSPECTOR_H
#define BREAKPOINTINSPECTOR_H

#include <QDockWidget>
#include <QFrame>
#include <QGridLayout>

#include "breakpointdialog.h"

class BreakpointInspector : public QDockWidget
{
   Q_OBJECT
public:
   BreakpointInspector();
   virtual ~BreakpointInspector();

public slots:
   void showMe();

protected:
   BreakpointDialog* dialog;
   QFrame* frame;
};

#endif // BREAKPOINTINSPECTOR_H
