#ifndef ICENTERWIDGETITEM_H
#define ICENTERWIDGETITEM_H

#include "iprojecttreeviewitem.h"

class ICenterWidgetItem
{
public:
   void InitTabItem(IProjectTreeViewItem* link = 0)
   {
      treeLink = link;
   }

   virtual bool isModified() = 0;
   virtual void setModified(bool modified) = 0;

   virtual bool onCloseQuery() = 0;
   virtual void onClose() = 0;
   virtual bool onSaveQuery() = 0;
   virtual void onSave() = 0;
// CPTODO later:   virtual bool canChangeName() = 0;
// CPTODO later:   virtual bool onNameChanged(QString newValue) = 0;

protected:
   IProjectTreeViewItem* treeLink;
};

#endif // ICENTERWIDGETITEM_H
