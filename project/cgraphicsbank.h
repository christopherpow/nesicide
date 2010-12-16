#ifndef CGRAPHICSBANK_H
#define CGRAPHICSBANK_H

#include "cprojectbase.h"
#include "graphicsbankeditorform.h"

#include <QMessageBox>
#include <QMenu>

class CGraphicsBank : public CProjectBase
{
public:
   CGraphicsBank();
   virtual ~CGraphicsBank();

   QString getBankName();
   void setBankName(QString newName);

   QList<IChrRomBankItem*>bankItems;

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node);

   // IProjectTreeViewItem Interface Implmentation
   QString caption() const;
   virtual void contextMenuEvent(QContextMenuEvent* event, QTreeView* parent);
   virtual void openItemEvent(QTabWidget* tabWidget);
   virtual bool onCloseQuery();
   virtual void onClose();
   virtual int getTabIndex();
   virtual void onSaveDocument();
   virtual bool onNameChanged(QString newName);
   virtual bool canChangeName()
   {
      return true;
   }
   virtual bool isDocumentSaveable()
   {
      return true;
   }

private:
   bool m_isModified;
   int m_tabIndex;
   QString m_name;
   GraphicsBankEditorForm* m_editor;
};

#endif // CGRAPHICSBANK_H
