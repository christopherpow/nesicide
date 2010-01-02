#ifndef REGISTERINSPECTOR_H
#define REGISTERINSPECTOR_H

#include <QDockWidget>
#include <QFrame>
#include <QGridLayout>

#include "registerdisplaydialog.h"

class RegisterInspector : public QDockWidget
{
   Q_OBJECT
public:
   RegisterInspector(eMemoryType display);
   virtual ~RegisterInspector();
protected:
   RegisterDisplayDialog* dialog;
   QFrame* frame;
};

#endif // REGISTERINSPECTOR_H
