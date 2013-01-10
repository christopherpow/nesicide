#ifndef CATTRIBUTETABLE_H
#define CATTRIBUTETABLE_H

#include <stdint.h>

#include "cprojectbase.h"
#include "attributetableeditorform.h"

#include <QMessageBox>
#include <QTabWidget>

class CAttributeTable : public CProjectBase
{
   Q_OBJECT
public:
   CAttributeTable(IProjectTreeViewItem* parent);
   virtual ~CAttributeTable();

   // Member getters
   QList<uint8_t>& getPalette();

   AttributeTableEditorForm* editor() { return dynamic_cast<AttributeTableEditorForm*>(m_editor); }

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors);

   // IProjectTreeViewItem Interface Implmentation
   virtual void openItemEvent(CProjectTabWidget* parent);
   virtual void saveItemEvent();
   virtual bool canChangeName() { return true; }
   virtual bool onNameChanged(QString newName);

private:
   QList<uint8_t> m_palette;
};

#endif // CATTRIBUTETABLE_H
