#ifndef CHRMEMINSPECTOR_H
#define CHRMEMINSPECTOR_H

#include "cdebuggerbase.h"

#include <QFrame>
#include <QGridLayout>

#include "chrromdisplaydialog.h"

class CHRMEMInspector : public CDebuggerBase
{
   Q_OBJECT
public:
   CHRMEMInspector(QWidget *parent = 0);
   virtual ~CHRMEMInspector();
protected:
   CHRROMDisplayDialog* dialog;
   QFrame* frame;
};

#endif // CHRMEMINSPECTOR_H
