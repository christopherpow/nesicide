#ifndef ATTRIBUTETABLEEDITORFORM_H
#define ATTRIBUTETABLEEDITORFORM_H

#include <stdint.h>

#include "cdesignereditorbase.h"
#include "iprojecttreeviewitem.h"
#include "qtcolorpicker.h"

namespace Ui
{
class AttributeTableEditorForm;
}

class AttributeTableEditorForm : public CDesignerEditorBase
{
   Q_OBJECT
public:
   AttributeTableEditorForm(QList<uint8_t> palette,IProjectTreeViewItem* link = 0,QWidget* parent = 0);
   virtual ~AttributeTableEditorForm();

   QList<uint8_t> attributeTable() { return m_palette; }

protected:
   void changeEvent(QEvent* event);
   void contextMenuEvent(QContextMenuEvent *event);

private:
   Ui::AttributeTableEditorForm* ui;
   QList<uint8_t> m_palette;
   QList<QtColorPicker*> m_colors;

private slots:
   void colorChanged(QColor color);
};

#endif // ATTRIBUTETABLEEDITORFORM_H
