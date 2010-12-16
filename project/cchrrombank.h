#ifndef CCHRROMBANK_H
#define CCHRROMBANK_H

#include "cprojectbase.h"
#include "chrromdisplaydialog.h"

class CCHRROMBank : public CProjectBase
{
public:
   CCHRROMBank();
   virtual ~CCHRROMBank();

   qint8 bankID;
   qint8* data;
   CHRROMDisplayDialog* editor;
   qint8 tabId;

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
   virtual bool isDocumentSaveable()
   {
      return false;
   };
   virtual void onSaveDocument();
   virtual bool canChangeName()
   {
      return false;
   };
   virtual bool onNameChanged(QString)
   {
      return true;
   };
};

#endif // CCHRROMBANK_H
