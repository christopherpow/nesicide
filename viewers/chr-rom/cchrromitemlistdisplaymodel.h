#ifndef CCHRROMITEMLISTDISPLAYMODEL_H
#define CCHRROMITEMLISTDISPLAYMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include "ichrrombankitem.h"
#include "iprojecttreeviewitem.h"

enum
{
   ChrRomBankItemCol_Icon,
   ChrRomBankItemCol_Name,
   ChrRomBankItemCol_Type,
   ChrRomBankItemCol_Size,
   ChrRomBankItemCol_MAX
};

class CChrRomItemListDisplayModel : public QAbstractTableModel
{
   Q_OBJECT
public:
   CChrRomItemListDisplayModel(QObject* parent = 0);
   virtual ~CChrRomItemListDisplayModel();
   QVariant data(const QModelIndex& index, int role) const;
   Qt::ItemFlags flags(const QModelIndex& index) const;
   bool setData ( const QModelIndex& index, const QVariant& value, int );
   QVariant headerData(int section, Qt::Orientation orientation,
                       int role = Qt::DisplayRole) const;
   int rowCount(const QModelIndex& parent = QModelIndex()) const;
   int columnCount(const QModelIndex& parent = QModelIndex()) const;
   void removeRow(int row, const QModelIndex &parent);
   void insertRow(IChrRomBankItem* item, const QModelIndex &parent);
   QList<IChrRomBankItem*> bankItems() { return chrRomBankItems; }
   void setBankItems(QList<IChrRomBankItem*> items) { chrRomBankItems = items; }

public slots:
   void update();

private:
   QList<IChrRomBankItem*> chrRomBankItems;
};

#endif // CCHRROMITEMLISTDISPLAYMODEL_H
