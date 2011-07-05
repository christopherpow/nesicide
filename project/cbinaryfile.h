#ifndef CBINARYFILE_H
#define CBINARYFILE_H

#include "cprojectbase.h"
#include "ichrrombankitem.h"

#include <QMessageBox>

class CBinaryFile : public CProjectBase,
   public IChrRomBankItem
{
   Q_OBJECT
public:
   CBinaryFile(IProjectTreeViewItem* parent);
   virtual ~CBinaryFile();

   QByteArray getBinaryData();
   void setBinaryData(const QByteArray& newBinaryData);

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors);
   virtual bool deserializeContent();

   // IProjectTreeViewItem Interface Implmentation
   QString caption() const;
   virtual void contextMenuEvent(QContextMenuEvent* event, QTreeView* parent);
   virtual void openItemEvent(CProjectTabWidget*) {}
   virtual bool onCloseQuery()
   {
      return true;
   }
   virtual void onClose() {}
   virtual void saveItemEvent() {}
   virtual bool canChangeName()
   {
      return true;
   }
   virtual bool onNameChanged(QString newName);

   // IChrRomBankItem Interface Implementation
   virtual int getChrRomBankItemSize();
   virtual QByteArray getChrRomBankItemData();
   virtual QIcon getChrRomBankItemIcon();
   virtual QString getItemType()
   {
      return "Binary File";
   };

private:
   // Attributes
   QByteArray m_binaryData;
};

#endif // CBINARYFILE_H
