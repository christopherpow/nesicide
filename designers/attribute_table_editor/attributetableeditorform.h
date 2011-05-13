#ifndef ATTRIBUTETABLEEDITORFORM_H
#define ATTRIBUTETABLEEDITORFORM_H

#include "cdesignereditorbase.h"

#include "iprojecttreeviewitem.h"

namespace Ui
{
class AttributeTableEditorForm;
}

class AttributeTableEditorForm : public CDesignerEditorBase
{
   Q_OBJECT
public:
   AttributeTableEditorForm(IProjectTreeViewItem* link = 0,QWidget* parent = 0);
   ~AttributeTableEditorForm();

protected:
   void changeEvent(QEvent* e);

private:
   Ui::AttributeTableEditorForm* ui;

private slots:
};

#endif // ATTRIBUTETABLEEDITORFORM_H
