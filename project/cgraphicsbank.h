#ifndef CGRAPHICSBANK_H
#define CGRAPHICSBANK_H

#include "cprojectbase.h"
#include "graphicsbankeditorform.h"

#include <QMessageBox>
#include <QMenu>

class CGraphicsBank : public CProjectBase
{
public:
   CGraphicsBank(IProjectTreeViewItem* parent);
   virtual ~CGraphicsBank();

   QList<IChrRomBankItem*>& getGraphics() { return m_bankItems; }

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors);

   // IProjectTreeViewItem Interface Implmentation
   QString caption() const;
   virtual void contextMenuEvent(QContextMenuEvent* event, QTreeView* parent);
   virtual void openItemEvent(QTabWidget* tabWidget);
   virtual void onSaveDocument();
   virtual bool onNameChanged(QString newName);
   virtual bool canChangeName()
   {
      return true;
   }

private:
   // Attributes
   QList<IChrRomBankItem*> m_bankItems;

   // Designer
   GraphicsBankEditorForm* m_editor;
};

#endif // CGRAPHICSBANK_H
