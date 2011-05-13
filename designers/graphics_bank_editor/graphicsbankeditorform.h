#ifndef GRAPHICSBANKEDITORFORM_H
#define GRAPHICSBANKEDITORFORM_H

#include <QList>

#include "cdesignereditorbase.h"
#include "graphicsbankadditemsdialog.h"
#include "cchrromitemlistdisplaymodel.h"
#include "ichrrombankitem.h"
#include "iprojecttreeviewitem.h"

namespace Ui
{
class GraphicsBankEditorForm;
}

class GraphicsBankEditorForm : public CDesignerEditorBase
{
   Q_OBJECT
public:
   GraphicsBankEditorForm(IProjectTreeViewItem* link = 0,QWidget* parent = 0);
   ~GraphicsBankEditorForm();
   QList<IChrRomBankItem*> chrRomBankItems;
   void updateChrRomBankItemList(QList<IChrRomBankItem*> newList);

protected:
   void changeEvent(QEvent* e);

private:
   Ui::GraphicsBankEditorForm* ui;
   CChrRomItemListDisplayModel* model;

private slots:
   void on_AddItemPushButton_clicked();
   void on_removeItemPushButton_clicked();
};

#endif // GRAPHICSBANKEDITORFORM_H
