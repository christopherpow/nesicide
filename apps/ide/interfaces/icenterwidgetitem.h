#ifndef ICENTERWIDGETITEM_H
#define ICENTERWIDGETITEM_H

#include "iprojecttreeviewitem.h"

#include <QMessageBox>

class ICenterWidgetItem
{
public:
   void InitTabItem(IProjectTreeViewItem* link = 0)
   {
      _treeLink = link;
   }

   virtual bool isModified() = 0;
   virtual void setModified(bool modified) = 0;

   virtual QMessageBox::StandardButton onCloseQuery() = 0;
   virtual void onClose() = 0;
   virtual void onSave() = 0;

   virtual QMenu& editorMenu() = 0;

   virtual IProjectTreeViewItem* treeLink() { return _treeLink; }

protected:
   IProjectTreeViewItem* _treeLink;
};

#endif // ICENTERWIDGETITEM_H
