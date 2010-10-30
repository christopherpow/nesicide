#ifndef CODEINSPECTOR_H
#define CODEINSPECTOR_H

#include <QDockWidget>
#include <QFrame>
#include <QGridLayout>

#include "codebrowserdialog.h"

class CodeInspector : public QDockWidget
{
   Q_OBJECT
public:
   CodeInspector();
   virtual ~CodeInspector();

public slots:
   void showMe();

protected:
   CodeBrowserDialog* dialog;
   QFrame* frame;
};

#endif // CODEINSPECTOR_H
