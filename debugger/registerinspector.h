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

public slots:
   void showMe(eMemoryType display);

protected:
   RegisterDisplayDialog* dialog;
   QFrame* frame;
   eMemoryType m_display;
};

#endif // REGISTERINSPECTOR_H
