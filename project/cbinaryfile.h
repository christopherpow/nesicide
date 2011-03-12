#ifndef CBINARYFILE_H
#define CBINARYFILE_H

#include "cprojectbase.h"
#include "ichrrombankitem.h"

#include <QMessageBox>

class CBinaryFile : public CProjectBase,
   public IChrRomBankItem
{
public:
   CBinaryFile(IProjectTreeViewItem* parent);
   virtual ~CBinaryFile();

   QByteArray* getBinaryData();
   void setBinaryData(QByteArray* newBinaryData);

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors);

   // IProjectTreeViewItem Interface Implmentation
   QString caption() const;
   virtual void contextMenuEvent(QContextMenuEvent* event, QTreeView* parent);
   virtual void openItemEvent(QTabWidget*) {}
   virtual bool onCloseQuery()
   {
      return true;
   }
   virtual void onClose() {}
   virtual bool isDocumentSaveable()
   {
      return false;
   }
   virtual void onSaveDocument() {}
   virtual bool canChangeName()
   {
      return true;
   }
   virtual bool onNameChanged(QString newName);

   // IChrRomBankItem Interface Implementation
   virtual int getChrRomBankItemSize();
   virtual QByteArray* getChrRomBankItemData();
   virtual QIcon getChrRomBankItemIcon();
   virtual QString getItemType()
   {
      return "Binary File";
   };

private:
   // Attributes
   QByteArray* m_binaryData;
};

#endif // CBINARYFILE_H
