#ifndef NAMETABLEINSPECTOR_H
#define NAMETABLEINSPECTOR_H

#include <QDockWidget>
#include <QFrame>
#include <QGridLayout>

#include "nametabledisplaydialog.h"

class NameTableInspector : public QDockWidget {
   Q_OBJECT
public:
   NameTableInspector();
   virtual ~NameTableInspector();
protected:
   NameTableDisplayDialog* dialog;
   QFrame* frame;
};

#endif // NAMETABLEINSPECTOR_H
