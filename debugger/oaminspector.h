#ifndef OAMINSPECTOR_H
#define OAMINSPECTOR_H

#include <QDockWidget>
#include <QFrame>
#include <QGridLayout>

#include "oamdisplaydialog.h"

class OAMInspector : public QDockWidget {
   Q_OBJECT
public:
   OAMInspector();
   virtual ~OAMInspector();
protected:
   OAMDisplayDialog* dialog;
   QFrame* frame;
};

#endif // OAMINSPECTOR_H
