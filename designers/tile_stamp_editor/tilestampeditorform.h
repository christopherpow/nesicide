#ifndef TILESTAMPEDITORFORM_H
#define TILESTAMPEDITORFORM_H

#include "cdesignereditorbase.h"

#include "ctilestamprenderer.h"

namespace Ui {
   class TileStampEditorForm;
}

class TileStampEditorForm : public CDesignerEditorBase
{
   Q_OBJECT

public:
   explicit TileStampEditorForm(QByteArray data,int xSize,int ySize,IProjectTreeViewItem* link,QWidget *parent = 0);
   virtual ~TileStampEditorForm();

   QByteArray tile() { return m_tile; }
   void currentSize(int* xSize,int* ySize);

protected:
   void changeEvent(QEvent *event);
   void contextMenuEvent(QContextMenuEvent *event);
   void resizeEvent(QResizeEvent *event);
   void renderer_mousePressEvent(QMouseEvent *event);
   void renderer_mouseMoveEvent(QMouseEvent *event);
   void renderer_mouseReleaseEvent(QMouseEvent *event);
   bool eventFilter(QObject *obj, QEvent *event);

private slots:
   void ySize_currentIndexChanged(int index);
   void xSize_currentIndexChanged(int index);
   void colorPicked();
   void on_verticalScrollBar_valueChanged(int value);
   void on_horizontalScrollBar_valueChanged(int value);
   void on_zoomSlider_valueChanged(int value);

private:
   Ui::TileStampEditorForm *ui;
   CTileStampRenderer* renderer;
   QByteArray m_tile;
   int m_xSize;
   int m_ySize;
   char* imgData;
   void updateScrollbars();

public slots:
   void renderData();
};

#endif // TILESTAMPEDITORFORM_H
