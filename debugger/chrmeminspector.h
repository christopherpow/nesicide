#ifndef CHRMEMINSPECTOR_H
#define CHRMEMINSPECTOR_H

#include <QDockWidget>
#include <QFrame>
#include <QGridLayout>

#include "chrromdisplaydialog.h"

class CHRMEMInspector : public QDockWidget
{
   Q_OBJECT
public:
   CHRMEMInspector();
   virtual ~CHRMEMInspector();
protected:
   CHRROMDisplayDialog* dialog;
   QFrame* frame;
};

#endif // CHRMEMINSPECTOR_H
