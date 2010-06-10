#ifndef APUINFORMATIONINSPECTOR_H
#define APUINFORMATIONINSPECTOR_H

#include <QDockWidget>
#include <QFrame>
#include <QGridLayout>

#include "apuinformationdialog.h"

class APUInformationInspector : public QDockWidget
{
   Q_OBJECT
public:
   APUInformationInspector();
   virtual ~APUInformationInspector();

public slots:
   void showMe();

protected:
   APUInformationDialog* dialog;
   QFrame* frame;
};

#endif // PPUINFORMATIONINSPECTOR_H
