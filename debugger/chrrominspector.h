#ifndef CHRROMINSPECTOR_H
#define CHRROMINSPECTOR_H

#include <QDockWidget>
#include <QFrame>
#include <QGridLayout>

#include "chrromdisplaydialog.h"

class CHRROMInspector : public QDockWidget {
   Q_OBJECT
public:
   CHRROMInspector();
   virtual ~CHRROMInspector();
protected:
   CHRROMDisplayDialog* dialog;
   QFrame* frame;
};

#endif // CHRROMINSPECTOR_H
