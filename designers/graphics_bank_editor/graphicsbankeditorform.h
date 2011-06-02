#ifndef GRAPHICSBANKEDITORFORM_H
#define GRAPHICSBANKEDITORFORM_H

#include <QList>

#include "cdesignereditorbase.h"
#include "cchrromitemlistdisplaymodel.h"
#include "cchrrombankitemdelegate.h"
#include "ichrrombankitem.h"
#include "iprojecttreeviewitem.h"

#include "cchrrompreviewrenderer.h"

namespace Ui
{
class GraphicsBankEditorForm;
}

class GraphicsBankEditorForm : public CDesignerEditorBase
{
   Q_OBJECT
public:
   GraphicsBankEditorForm(QList<IChrRomBankItem*> bankItems,IProjectTreeViewItem* link = 0,QWidget* parent = 0);
   ~GraphicsBankEditorForm();
   void updateChrRomBankItemList(QList<IChrRomBankItem*> newList);
   QList<IChrRomBankItem*> bankItems();
   void setBankItems(QList<IChrRomBankItem*> items);

protected:
   void changeEvent(QEvent* e);
   void keyPressEvent(QKeyEvent *e);
   void resizeEvent(QResizeEvent* event);

private:
   Ui::GraphicsBankEditorForm* ui;
   CChrRomItemListDisplayModel* model;
   CCHRROMPreviewRenderer* renderer;
   CChrRomBankItemDelegate* delegate;
   char* imgData;
   void renderData();
   void updateScrollbars();

private slots:
   void updateUi();
   void on_verticalScrollBar_valueChanged(int value);
   void on_horizontalScrollBar_valueChanged(int value);
   void on_horizontalScrollBar_actionTriggered(int action);
   void on_verticalScrollBar_actionTriggered(int action);
   void on_zoomSlider_valueChanged(int value);
   void on_zoomSlider_actionTriggered(int action);
   void on_zoomSlider_sliderMoved(int position);
   void colorChanged (const QColor& color);
};

#endif // GRAPHICSBANKEDITORFORM_H
