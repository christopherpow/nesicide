#ifndef ICHRROMBANKITEM_H
#define ICHRROMBANKITEM_H

#include <QIcon>
#include <QByteArray>

class IChrRomBankItem
{
public:
   virtual int getChrRomBankItemSize() = 0;
   virtual QByteArray* getChrRomBankItemData() = 0;
   virtual QIcon getChrRomBankItemIcon() = 0;
   virtual QString getItemType() = 0;
};

#endif // ICHRROMBANKITEM_H
