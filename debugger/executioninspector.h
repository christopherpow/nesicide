#ifndef EXECUTIONINSPECTOR_H
#define EXECUTIONINSPECTOR_H

#include <QDockWidget>
#include <QFrame>
#include <QGridLayout>

#include "executiontracerdialog.h"

class ExecutionInspector : public QDockWidget
{
   Q_OBJECT
public:
   ExecutionInspector();
   virtual ~ExecutionInspector();
protected:
   ExecutionTracerDialog* dialog;
   QFrame* frame;
};

#endif // EXECUTIONINSPECTOR_H
