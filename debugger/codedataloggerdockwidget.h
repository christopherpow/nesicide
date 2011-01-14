#ifndef CODEDATALOGGERDOCKWIDGET_H
#define CODEDATALOGGERDOCKWIDGET_H

#include "ccodedataloggerrenderer.h"

#include <QDockWidget>

namespace Ui {
   class CodeDataLoggerDockWidget;
}

class CodeDataLoggerDockWidget : public QDockWidget
{
   Q_OBJECT

public:
   explicit CodeDataLoggerDockWidget(QWidget *parent = 0);
   ~CodeDataLoggerDockWidget();

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
   
public slots:
   void renderData();

private slots:
   void on_displaySelect_currentIndexChanged(int index);
   void on_verticalScrollBar_valueChanged(int value);
   void on_horizontalScrollBar_valueChanged(int value);
   void on_zoomSlider_valueChanged(int value);
};

#endif // CODEDATALOGGERDOCKWIDGET_H
