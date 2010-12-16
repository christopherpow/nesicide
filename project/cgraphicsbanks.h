#ifndef CGRAPHICSBANKS_H
#define CGRAPHICSBANKS_H

#include "cprojectbase.h"
#include "cgraphicsbank.h"
#include "cprojecttreeviewmodel.h"

#include <QList>
#include <QInputDialog>

class CGraphicsBanks : public CProjectBase
{
public:
   CGraphicsBanks();
   virtual ~CGraphicsBanks();

   QList<CGraphicsBank*> *getGraphicsBankArray();
   void setGraphicsBankArray(QList<CGraphicsBank*> *newGraphicsBankArray);

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node);

   // IProjectTreeViewItem Interface Implmentation
   QString caption() const
   {
      return QString("Graphics Banks");
   }
   virtual void contextMenuEvent(QContextMenuEvent* event, QTreeView* parent);
   virtual void openItemEvent(QTabWidget*) {}
   virtual bool onCloseQuery()
   {
      return true;
   }
   virtual void onClose() {}
   virtual int getTabIndex()
   {
      return -1;
   }
   virtual bool isDocumentSaveable()
   {
      return false;
   }
   virtual void onSaveDocument() {}
   virtual bool canChangeName()
   {
      return false;
   }
   virtual bool onNameChanged(QString)
   {
      return true;
   }

private:
   QList<CGraphicsBank*> *m_pGraphicsBank;
};

#endif // CGRAPHICSBANKS_H
