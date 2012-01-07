#ifndef GRAPHICSBANKEDITORFORM_H
#define GRAPHICSBANKEDITORFORM_H

#include <QList>

#include "cdesignereditorbase.h"
#include "cchrromitemtabledisplaymodel.h"
#include "cchrrombankitemdelegate.h"
#include "ichrrombankitem.h"
#include "iprojecttreeviewitem.h"

#include "panzoomrenderer.h"

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
   void keyPressEvent(QKeyEvent *event);

private:
   Ui::GraphicsBankEditorForm* ui;
   CChrRomItemTableDisplayModel* model;
   PanZoomRenderer* renderer;
   CChrRomBankItemDelegate* delegate;
   char* imgData;

private slots:
   void renderData();
   void updateUi();
   void snapTo(QString item);
   void applyChangesToTab(QString uuid);
   void applyProjectPropertiesToTab();
};

#endif // GRAPHICSBANKEDITORFORM_H
