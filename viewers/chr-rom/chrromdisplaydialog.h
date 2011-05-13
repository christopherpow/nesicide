#ifndef CHRROMDISPLAYDIALOG_H
#define CHRROMDISPLAYDIALOG_H

#include "cdesignereditorbase.h"
#include "cchrrompreviewrenderer.h"

#include "iprojecttreeviewitem.h"

namespace Ui
{
class CHRROMDisplayDialog;
}

class CHRROMDisplayDialog : public CDesignerEditorBase
{
   Q_OBJECT
public:
   CHRROMDisplayDialog(bool usePPU,qint8* data,IProjectTreeViewItem* link = 0,QWidget* parent = 0);
   ~CHRROMDisplayDialog();
   void updateScrollbars();

protected:
   CCHRROMPreviewRenderer* renderer;
   qint8* chrrom;

protected:
   void showEvent(QShowEvent* event);
   void hideEvent(QHideEvent* event);
   void changeEvent(QEvent* e);
   void resizeEvent(QResizeEvent* event);

private:
   Ui::CHRROMDisplayDialog* ui;
   char* imgData;
   bool m_usePPU;

public slots:
   void renderData();

private slots:
   void on_exportPushButton_clicked();
   void on_updateScanline_editingFinished();
   void on_verticalScrollBar_valueChanged(int value);
   void on_horizontalScrollBar_valueChanged(int value);
   void on_horizontalScrollBar_actionTriggered(int action);
   void on_verticalScrollBar_actionTriggered(int action);
   void on_zoomSlider_valueChanged(int value);
   void on_zoomSlider_actionTriggered(int action);
   void on_zoomSlider_sliderMoved(int position);
   void colorChanged (const QColor& color);
};

#endif // CHRROMDISPLAYDIALOG_H
