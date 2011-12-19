#ifndef CHRROMDISPLAYDIALOG_H
#define CHRROMDISPLAYDIALOG_H

#include "cdesignereditorbase.h"
#include "cchrrompreviewrenderer.h"

#include "iprojecttreeviewitem.h"

#include "emulator_core.h"

#include "debuggerupdatethread.h"

namespace Ui
{
class CHRROMDisplayDialog;
}

class CHRROMDisplayDialog : public CDesignerEditorBase
{
   Q_OBJECT
public:
   CHRROMDisplayDialog(bool usePPU,qint8* data,IProjectTreeViewItem* link = 0,QWidget* parent = 0);
   virtual ~CHRROMDisplayDialog();

protected:
   CCHRROMPreviewRenderer* renderer;
   qint8 chrrom[MEM_8KB];

protected:
   void changeEvent(QEvent* event);
   void showEvent(QShowEvent* event);
   void hideEvent(QHideEvent* event);
   void resizeEvent(QResizeEvent* event);
   void contextMenuEvent(QContextMenuEvent *event);

private:
   Ui::CHRROMDisplayDialog* ui;
   char* imgData;
   bool m_usePPU;
   DebuggerUpdateThread* pThread;
   void updateScrollbars();

public slots:
   void renderData();

private slots:
   void on_exportPushButton_clicked();
   void on_updateScanline_editingFinished();
   void on_verticalScrollBar_valueChanged(int value);
   void on_horizontalScrollBar_valueChanged(int value);
   void on_zoomSlider_valueChanged(int value);
   void colorChanged (const QColor& color);
   void applyProjectPropertiesToTab();
};

#endif // CHRROMDISPLAYDIALOG_H
