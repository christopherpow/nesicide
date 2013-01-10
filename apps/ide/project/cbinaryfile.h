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

   // Member getters.
   QByteArray getBinaryData();
   QImage getBinaryImage();
   void getSize(int* xSize,int* ySize) { (*xSize) = m_xSize; (*ySize) = m_ySize; }

   // Member setters.
   void setBinaryData(const QByteArray& newBinaryData);

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors);
   virtual bool deserializeContent();

   // IProjectTreeViewItem Interface Implmentation
   QString caption() const;
   virtual void openItemEvent(CProjectTabWidget*) {}

   // ICenterWidgetItem Interface Implementation

   // IChrRomBankItem Interface Implementation
   virtual int getChrRomBankItemSize();
   virtual QByteArray getChrRomBankItemData();
   virtual QIcon getChrRomBankItemIcon();
   virtual QImage getChrRomBankItemImage();
   virtual QString getItemType()
   {
      return "Binary File";
   }

private:
   // Attributes
   QByteArray m_binaryData;
   int m_xSize;
   int m_ySize;
};

#endif // CBINARYFILE_H
