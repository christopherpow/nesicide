#ifndef CCHRROMITEMLISTDISPLAYMODEL_H
#define CCHRROMITEMLISTDISPLAYMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include "ichrrombankitem.h"
#include "iprojecttreeviewitem.h"
class CChrRomItemListDisplayModel : public QAbstractTableModel
{
   Q_OBJECT
public:
   CChrRomItemListDisplayModel(QObject* parent);
   virtual ~CChrRomItemListDisplayModel();
   QVariant data(const QModelIndex &index, int role) const;
   Qt::ItemFlags flags(const QModelIndex &index) const;
   bool setData ( const QModelIndex & index, const QVariant & value, int );
   QVariant headerData(int section, Qt::Orientation orientation,
                     int role = Qt::DisplayRole) const;
   QModelIndex index(int row, int column,
                   const QModelIndex &parent = QModelIndex()) const;
   int rowCount(const QModelIndex &parent = QModelIndex()) const;
   int columnCount(const QModelIndex &parent = QModelIndex()) const;
   void layoutChangedEvent();
   QList<IChrRomBankItem*> chrRomBankItems;

};

#endif // CCHRROMITEMLISTDISPLAYMODEL_H
