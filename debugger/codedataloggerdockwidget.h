#ifndef CODEDATALOGGERDOCKWIDGET_H
#define CODEDATALOGGERDOCKWIDGET_H

#include "cdebuggerbase.h"

#include "ccodedataloggerrenderer.h"

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
   void resizeEvent(QResizeEvent* event);
   void updateScrollbars();
   CCodeDataLoggerRenderer* renderer;

private:
   Ui::CodeDataLoggerDockWidget *ui;
   char* cpuImgData;
   char* ppuImgData;
   DebuggerUpdateThread* pThread;

public slots:
   void renderData();

private slots:
   void on_exportData_clicked();
   void on_displaySelect_currentIndexChanged(int index);
   void on_verticalScrollBar_valueChanged(int value);
   void on_horizontalScrollBar_valueChanged(int value);
   void on_zoomSlider_valueChanged(int value);
};

#endif // CODEDATALOGGERDOCKWIDGET_H
