#ifndef CODEDATALOGGERDOCKWIDGET_H
#define CODEDATALOGGERDOCKWIDGET_H

#include "cdebuggerbase.h"

#include "panzoomrenderer.h"

#include "debuggerupdatethread.h"

enum
{
   CodeDataLogger_CPU,
   CodeDataLogger_PPU,
   CodeDataLogger_PRGROM // Not done yet.
};

namespace Ui {
   class CodeDataLoggerDockWidget;
}

class CodeDataLoggerDockWidget : public CDebuggerBase
{
   Q_OBJECT

public:
   explicit CodeDataLoggerDockWidget(QWidget *parent = 0);
   virtual ~CodeDataLoggerDockWidget();

protected:
   void showEvent(QShowEvent* event);
   void hideEvent(QHideEvent* event);
   void changeEvent(QEvent* e);

private:
   Ui::CodeDataLoggerDockWidget *ui;
   PanZoomRenderer* renderer;
   DebuggerUpdateThread* pThread;
   QPoint pressPos;

public slots:
   void renderData();
   void updateTargetMachine(QString target);

private slots:
   void on_exportData_clicked();
   void on_displaySelect_currentIndexChanged(int index);
};

#endif // CODEDATALOGGERDOCKWIDGET_H
