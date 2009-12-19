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
   CHRROMDisplayDialog* dialog;
   QFrame* frame;

private:
    char *imgData;
};

#endif // CHRROMINSPECTOR_H
