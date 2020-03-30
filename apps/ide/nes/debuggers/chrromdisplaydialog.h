#ifndef CHRROMDISPLAYDIALOG_H
#define CHRROMDISPLAYDIALOG_H

#include <QLabel>

#include "cdesignereditorbase.h"
#include "panzoomrenderer.h"

#include "iprojecttreeviewitem.h"

#include "nes_emulator_core.h"

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
   qint8 chrrom[MEM_8KB];

protected:
   void changeEvent(QEvent* event);
   void showEvent(QShowEvent* event);
   void hideEvent(QHideEvent* event);
   void updateInfoText(int x=-1,int y=-1);
   bool eventFilter(QObject *obj, QEvent *event);
   void renderer_enterEvent(QEvent* event);
   void renderer_leaveEvent(QEvent* event);
   void renderer_mouseMoveEvent(QMouseEvent *event);

private:
   Ui::CHRROMDisplayDialog* ui;
   QLabel* info;
   int8_t* imgData;
   IProjectTreeViewItem *m_link;
   bool m_usePPU;
   DebuggerUpdateThread* pThread;
   PanZoomRenderer* renderer;

public slots:
   void renderData();
   void repaintNeeded();
   void updateTargetMachine(QString target);
   void applyProjectPropertiesToTab();
   void applyChangesToTab(QString /*uuid*/);
   void compiler_compileDone(bool bOk);

private slots:
   void on_exportPushButton_clicked();
   void on_updateScanline_editingFinished();
};

#endif // CHRROMDISPLAYDIALOG_H
