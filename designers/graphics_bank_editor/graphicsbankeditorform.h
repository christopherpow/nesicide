#ifndef GRAPHICSBANKEDITORFORM_H
#define GRAPHICSBANKEDITORFORM_H

#include <QList>

#include "cdesignereditorbase.h"
#include "cchrromitemtabledisplaymodel.h"
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
   virtual ~GraphicsBankEditorForm();
   void updateChrRomBankItemList(QList<IChrRomBankItem*> newList);

   // Member getters.
   QList<IChrRomBankItem*> bankItems();

protected:
   void changeEvent(QEvent* event);
   void contextMenuEvent(QContextMenuEvent *event);
   void keyPressEvent(QKeyEvent *event);
   void resizeEvent(QResizeEvent* event);

private:
   Ui::GraphicsBankEditorForm* ui;
   CChrRomItemTableDisplayModel* model;
   CCHRROMPreviewRenderer* renderer;
   CChrRomBankItemDelegate* delegate;
   char* imgData;
   void renderData();
   void updateScrollbars();

private slots:
   void updateUi();
   void on_verticalScrollBar_valueChanged(int value);
   void on_horizontalScrollBar_valueChanged(int value);
   void on_zoomSlider_valueChanged(int value);
   void colorChanged (const QColor& color);
   void snapTo(QString item);
   void applyChangesToTab(QString uuid);
   void applyProjectPropertiesToTab();
};

#endif // GRAPHICSBANKEDITORFORM_H
