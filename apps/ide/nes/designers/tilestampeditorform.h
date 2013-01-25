#ifndef TILESTAMPEDITORFORM_H
#define TILESTAMPEDITORFORM_H

#include <QToolButton>

#include "colorpushbutton.h"
#include "cdesignereditorbase.h"
#include "cdesignercommon.h"
#include "ctilestamprenderer.h"
#include "cpropertylistmodel.h"
#include "cpropertyvaluedelegate.h"

typedef enum
{
   Overlay_HollowBox,
   Overlay_FilledBox,
   Overlay_HollowCircle,
   Overlay_FilledCircle,
   Overlay_Line,
   Overlay_Erase,
   Overlay_PasteClipboard,
   Overlay_PasteSelection,
   Overlay_FloodFill
} OverlayType;

typedef enum
{
   Resize_None,
   Resize_Top,
   Resize_Bottom,
   Resize_Left,
   Resize_Right,
   Resize_TopLeft,
   Resize_TopRight,
   Resize_BottomLeft,
   Resize_BottomRight
} SelectionResizeMode;

namespace Ui {
   class TileStampEditorForm;
}

class CTileStamp;

class TileStampEditorForm : public CDesignerEditorBase
{
   Q_OBJECT

public:
   explicit TileStampEditorForm(QByteArray data,QByteArray attr,QString attrTblUUID,QList<PropertyItem> tileProperties,int xSize,int ySize,bool grid,IProjectTreeViewItem* link,QWidget *parent = 0);
   virtual ~TileStampEditorForm();

   QByteArray tileData(bool useOverlay=false);
   QByteArray attributeData(bool useOverlay=false);
   QList<PropertyItem> tileProperties() { return m_tileProperties; }
   void currentSize(int* xSize,int* ySize) { (*xSize) = m_xSize; (*ySize) = m_ySize; }
   QUuid currentAttributeTable() { return m_attrTblUUID; }
   bool isGridEnabled() { return m_gridEnabled; }

   void initializeTile(QByteArray tileData,QByteArray attrData);
   void paintNormal();
   void setCurrentSize(int xSize,int ySize) { m_xSize = xSize; m_ySize = ySize; }

   // ICenterWidgetItem interface
   void onSave();

protected:
   void changeEvent(QEvent *event);
   void contextMenuEvent(QContextMenuEvent *event);
   void resizeEvent(QResizeEvent *event);
   void keyPressEvent(QKeyEvent *event);
   void showEvent(QShowEvent *event);
   void hideEvent(QHideEvent *event);
   void renderer_enterEvent(QEvent* event);
   void renderer_leaveEvent(QEvent* event);
   void renderer_mousePressEvent(QMouseEvent *event);
   void renderer_mouseMoveEvent(QMouseEvent *event);
   void renderer_mouseReleaseEvent(QMouseEvent *event);
   void renderer_mouseDoubleClickEvent(QMouseEvent *event);
   bool eventFilter(QObject *obj, QEvent *event);
   void updateScrollbars();
   void recolorTiles(int pixx,int pixy,int newColor,bool force=false);
   void recolorClipboard(int boxX1,int boxY1,int boxX2,int boxY2);
   void paintOverlay(QByteArray overlayData,QByteArray overlayAttr,int overlayXSize,int overlayYSize,int boxX1,int boxY1,int boxX2,int boxY2);
   void paintOverlay(OverlayType type,int selectedColor,int boxX1,int boxY1,int boxX2,int boxY2);
   void paintOverlay(int selectedColor,int pixx,int pixy);
   void copySelectionToClipboard(int boxX1,int boxY1,int boxX2,int boxY2);
   void copyNormalToSelection(int boxX1,int boxY1,int boxX2,int boxY2);
   void copyOverlayToNormal();
   void clearSelection();
   void updateInfoText(int x=-1,int y=-1);
   void paintTool(QMouseEvent* event);
   void pencilTool(QMouseEvent* event);
   void tileTool(QMouseEvent* event);
   void filledBoxTool(QMouseEvent* event);
   void hollowBoxTool(QMouseEvent* event);
   void boxTool(QMouseEvent* event,OverlayType overlayType);
   void filledCircleTool(QMouseEvent* event);
   void hollowCircleTool(QMouseEvent* event);
   void circleTool(QMouseEvent* event,bool filled);
   void lineTool(QMouseEvent* event);
   void selectionTool(QMouseEvent* event);
   int getSelectedColor();

private slots:
   void propertyTableView_currentChanged(QModelIndex index,QModelIndex);
   void on_filledCircleTool_clicked();
   void on_hollowCircleTool_clicked();
   void on_hollowBoxTool_clicked();
   void on_filledBoxTool_clicked();
   void on_paintAttr_clicked();
   void on_selectionTool_clicked();
   void on_flipVertical_clicked();
   void on_flipHorizontal_clicked();
   void on_cwRotate_clicked();
   void on_ccwRotate_clicked();
   void on_lineTool_clicked();
   void on_pencilTool_clicked();
   void on_paintTool_clicked();
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
   void tilePropertyListModel_dataChanged(QModelIndex topLeft,QModelIndex bottomRight);
   void applyProjectPropertiesToTab();
   void tileSelected(QModelIndex index);
   void updateTargetMachine(QString /*target*/) {}

private:
   Ui::TileStampEditorForm *ui;
   CTileStampRenderer* renderer;
   CTileStampRenderer* previewer;
   CPropertyListModel* tilePropertyListModel;
   CPropertyValueDelegate* tilePropertyValueDelegate;
   QList<ColorPushButton*> m_colors;
   QList<uint8_t> m_colorIndexes;
   QList<PropertyItem> m_tileProperties;
   int m_xSize;
   int m_ySize;
   QString m_attrTblUUID;
   bool m_gridEnabled;
   char* imgData;
   char* colorData;
   char* colorDataOverlay;
   char* colorDataSelection;
   QToolButton* m_activeTool;
   QPoint m_anchor;
   bool m_selection;
   QRect m_selectionRect;
   bool m_selectionCaptured;
   SelectionResizeMode m_resizeMode;
   QByteArray m_oldTileData;
   QByteArray m_oldAttributeData;

public slots:
   void renderData();
};

class TileStampPaintCommand : public QUndoCommand
{
public:
   TileStampPaintCommand(TileStampEditorForm* pEditor,
                         QByteArray oldTileData,
                         QByteArray oldAttributeData,
                         QUndoCommand* parent = 0);
   virtual int id() const { return TILE_STAMP_PAINT_COMMAND; }
   virtual bool mergeWith(const QUndoCommand* command);
   virtual void redo();
   virtual void undo();

private:
   TileStampEditorForm* m_pEditor;
   QByteArray m_oldTileData;
   QByteArray m_oldAttributeData;
   QByteArray m_newTileData;
   QByteArray m_newAttributeData;
};

class TileStampResizeCommand : public QUndoCommand
{
public:
   TileStampResizeCommand(TileStampEditorForm* pEditor,
                         int oldXSize,
                         int oldYSize,
                         QUndoCommand* parent = 0);
   virtual int id() const { return TILE_STAMP_RESIZE_COMMAND; }
   virtual bool mergeWith(const QUndoCommand* command);
   virtual void redo();
   virtual void undo();

private:
   TileStampEditorForm* m_pEditor;
   int m_oldXSize;
   int m_oldYSize;
   int m_newXSize;
   int m_newYSize;
};

#endif // TILESTAMPEDITORFORM_H
