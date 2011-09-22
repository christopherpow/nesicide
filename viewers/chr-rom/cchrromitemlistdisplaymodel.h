#ifndef CCHRROMITEMLISTDISPLAYMODEL_H
#define CCHRROMITEMLISTDISPLAYMODEL_H

#include <QAbstractListModel>
#include <QList>
#include "ichrrombankitem.h"
#include "iprojecttreeviewitem.h"

class CChrRomItemListDisplayModel : public QAbstractListModel
{
   Q_OBJECT
public:
   CChrRomItemListDisplayModel(QObject* parent = 0);
   virtual ~CChrRomItemListDisplayModel();
   QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
   QVariant data(const QModelIndex& index, int role) const;
   Qt::ItemFlags flags(const QModelIndex& index) const;
   int rowCount(const QModelIndex& parent = QModelIndex()) const;
   int columnCount(const QModelIndex& parent = QModelIndex()) const;
   QList<IChrRomBankItem*> bankItems() { return chrRomBankItems; }
   void setBankItems(QList<IChrRomBankItem*> items) { chrRomBankItems = items; }

public slots:
   void update();

private:
   QList<IChrRomBankItem*> chrRomBankItems;
};

#endif // CCHRROMITEMLISTDISPLAYMODEL_H
