#ifndef MAPPERINFORMATIONINSPECTOR_H
#define MAPPERINFORMATIONINSPECTOR_H

#include <QDockWidget>
#include <QFrame>
#include <QGridLayout>

#include "mapperinformationdialog.h"

class MapperInformationInspector : public QDockWidget
{
   Q_OBJECT
public:
   MapperInformationInspector();
   virtual ~MapperInformationInspector();

public slots:
   void showMe();

protected:
   MapperInformationDialog* dialog;
   QFrame* frame;
};

#endif // MAPPERINFORMATIONINSPECTOR_H
