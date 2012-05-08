#ifndef CGRAPHICSBANK_H
#define CGRAPHICSBANK_H

#include "cprojectbase.h"
#include "graphicsbankeditorform.h"

#include <QMessageBox>
#include <QMenu>

class CGraphicsBank : public CProjectBase
{
   Q_OBJECT
public:
   CGraphicsBank(IProjectTreeViewItem* parent);
   virtual ~CGraphicsBank();

   // Member getters
   QList<IChrRomBankItem*> getGraphics();

   GraphicsBankEditorForm* editor() { return dynamic_cast<GraphicsBankEditorForm*>(m_editor); }
   void exportAsPNG();

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors);

   // IProjectTreeViewItem Interface Implmentation
   QString caption() const;
   virtual void contextMenuEvent(QContextMenuEvent* event, QTreeView* parent);
   virtual void openItemEvent(CProjectTabWidget* tabWidget);
   virtual void saveItemEvent();
   virtual bool canChangeName()
   {
      return true;
   }
   virtual bool onNameChanged(QString newName);

private:
   // Attributes
   QList<IChrRomBankItem*> m_leftBankItems;
   QList<IChrRomBankItem*> m_rightBankItems;
};

#endif // CGRAPHICSBANK_H
