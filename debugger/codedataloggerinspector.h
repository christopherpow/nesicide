#ifndef CODEDATALOGGERINSPECTOR_H
#define CODEDATALOGGERINSPECTOR_H

#include <QDockWidget>
#include <QFrame>
#include <QGridLayout>

#include "codedataloggerdialog.h"

class CodeDataLoggerInspector : public QDockWidget
{
   Q_OBJECT
public:
   CodeDataLoggerInspector();
   virtual ~CodeDataLoggerInspector();

public slots:
   void showMe();

protected:
   CodeDataLoggerDialog* dialog;
   QFrame* frame;
};


#endif // CODEDATALOGGERINSPECTOR_H
