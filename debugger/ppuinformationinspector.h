#ifndef PPUINFORMATIONINSPECTOR_H
#define PPUINFORMATIONINSPECTOR_H

#include <QDockWidget>
#include <QFrame>
#include <QGridLayout>

#include "ppuinformationdialog.h"

class PPUInformationInspector : public QDockWidget
{
   Q_OBJECT
public:
   PPUInformationInspector();
   virtual ~PPUInformationInspector();

public slots:
   void showMe();

protected:
   PPUInformationDialog* dialog;
   QFrame* frame;
};

#endif // PPUINFORMATIONINSPECTOR_H
