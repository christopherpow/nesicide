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

   // Member getters
   QList<IChrRomBankItem*>& getGraphics();

   GraphicsBankEditorForm* editor() { return dynamic_cast<GraphicsBankEditorForm*>(m_editor); }

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors);

   // IProjectTreeViewItem Interface Implmentation
   QString caption() const;
   virtual void contextMenuEvent(QContextMenuEvent* event, QTreeView* parent);
   virtual void openItemEvent(CProjectTabWidget* tabWidget);
   virtual void saveItemEvent();
   virtual bool onNameChanged(QString newName);
   virtual bool canChangeName()
   {
      return true;
   }

private:
   // Attributes
   QList<IChrRomBankItem*> m_bankItems;
};

#endif // CGRAPHICSBANK_H
