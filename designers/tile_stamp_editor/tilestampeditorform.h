#ifndef TILESTAMPEDITORFORM_H
#define TILESTAMPEDITORFORM_H

#include "colorpushbutton.h"
#include "cdesignereditorbase.h"
#include "ctilestamprenderer.h"
#include "cchrromitemlistdisplaymodel.h"

namespace Ui {
   class TileStampEditorForm;
}

class TileStampEditorForm : public CDesignerEditorBase
{
   Q_OBJECT

public:
   explicit TileStampEditorForm(QByteArray data,QString attrTblUUID,int xSize,int ySize,bool grid,IProjectTreeViewItem* link,QWidget *parent = 0);
   virtual ~TileStampEditorForm();

   QByteArray tileData();
   void currentSize(int* xSize,int* ySize) { (*xSize) = m_xSize; (*ySize) = m_ySize; }
   QUuid currentAttributeTable() { return m_attrTblUUID; }
   bool isGridEnabled() { return m_gridEnabled; }

protected:
   void changeEvent(QEvent *event);
   void contextMenuEvent(QContextMenuEvent *event);
   void resizeEvent(QResizeEvent *event);
   void renderer_enterEvent(QEvent* event);
   void renderer_leaveEvent(QEvent* event);
   void renderer_mousePressEvent(QMouseEvent *event);
   void renderer_mouseMoveEvent(QMouseEvent *event);
   void renderer_mouseReleaseEvent(QMouseEvent *event);
   void renderer_mouseDoubleClickEvent(QMouseEvent *event);
   bool eventFilter(QObject *obj, QEvent *event);
   void initializeTile(QByteArray tileData);
   void recolorTiles(int pixx,int pixy,int newColorTable);

private slots:
   void on_tileList_clicked(QModelIndex index);
   void on_tileList_activated(QModelIndex index);
   void on_paint_clicked();
   void on_clear_clicked();
   void grid_toggled(bool checked);
   void attributeTable_currentIndexChanged(int index);
   void ySize_currentIndexChanged(int index);
   void xSize_currentIndexChanged(int index);
   void colorPicked(bool value);
   void on_verticalScrollBar_valueChanged(int value);
   void on_horizontalScrollBar_valueChanged(int value);
   void on_zoomSlider_valueChanged(int value);
   void applyChangesToTab(QString uuid);

private:
   Ui::TileStampEditorForm *ui;
   CTileStampRenderer* renderer;
   CTileStampRenderer* previewer;
   CChrRomItemListDisplayModel* tileListModel;
   QByteArray m_tile;
   QList<ColorPushButton*> m_colors;
   int m_xSize;
   int m_ySize;
   QString m_attrTblUUID;
   bool m_gridEnabled;
   char* imgData;
   char* colorData;
   void updateScrollbars();

public slots:
   void renderData();
};

#endif // TILESTAMPEDITORFORM_H
