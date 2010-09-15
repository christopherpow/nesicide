#ifndef OUTPUTDOCKWIDGET_H
#define OUTPUTDOCKWIDGET_H

#include <QDockWidget>
#include <QFrame>
#include <QGridLayout>

#include "outputdialog.h"

class OutputDockWidget : public QDockWidget
{
   Q_OBJECT
public:
   OutputDockWidget();
   virtual ~OutputDockWidget();

protected:
   OutputDialog* dialog;
   QFrame* frame;
};

#endif // OUTPUTDOCKWIDGET_H
