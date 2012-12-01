#ifndef ATTRIBUTETABLEEDITORFORM_H
#define ATTRIBUTETABLEEDITORFORM_H

#include <stdint.h>

#include "cdesignereditorbase.h"
#include "iprojecttreeviewitem.h"
#include "qtcolorpicker.h"
#include "cdesignercommon.h"

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

   // Member getters
   QList<uint8_t> attributeTable() { return m_palette; }

   // Member setters
   void setPalette(int colorIdx,uint8_t color) { m_palette.replace(colorIdx,color); }

protected:
   void changeEvent(QEvent* event);
   void contextMenuEvent(QContextMenuEvent *event);
   void keyPressEvent(QKeyEvent *event);

private:
   Ui::AttributeTableEditorForm* ui;
   QList<uint8_t> m_palette;
   QList<QtColorPicker*> m_colors;

private slots:
   void colorChanged(QColor color);
   void applyProjectPropertiesToTab();
   void updateTargetMachine(QString /*target*/) {}
};

class AttributeTableChangeColorCommand : public QUndoCommand
{
public:
   AttributeTableChangeColorCommand(AttributeTableEditorForm* pEditor,
                         int colorIdx,
                         uint8_t oldColor,
                         QUndoCommand* parent = 0);
   virtual int id() const { return ATTRIBUTE_TABLE_CHAGE_COLOR_COMMAND; }
   virtual bool mergeWith(const QUndoCommand* command);
   virtual void redo();
   virtual void undo();

private:
   AttributeTableEditorForm* m_pEditor;
   int m_colorIdx;
   uint8_t m_oldColor;
   uint8_t m_newColor;
};

#endif // ATTRIBUTETABLEEDITORFORM_H
