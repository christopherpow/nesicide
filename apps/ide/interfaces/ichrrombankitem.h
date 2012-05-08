#ifndef ICHRROMBANKITEM_H
#define ICHRROMBANKITEM_H

#include <QIcon>
#include <QByteArray>
#include <iprojecttreeviewitem.h>

class IChrRomBankItem
{
public:
   virtual int getChrRomBankItemSize() = 0;
   virtual QByteArray getChrRomBankItemData() = 0;
   virtual QIcon getChrRomBankItemIcon() = 0;
   virtual QImage getChrRomBankItemImage() { return QImage(); }
   virtual QString getItemType() = 0;
};

#endif // ICHRROMBANKITEM_H
