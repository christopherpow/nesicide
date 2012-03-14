#ifndef CHRBANKITEMSTABWIDGET_H
#define CHRBANKITEMSTABWIDGET_H

#include <QTabWidget>

#include "ichrrombankitem.h"

namespace Ui {
class ChrBankItemsTabWidget;
}

class ChrBankItemsTabWidget : public QTabWidget
{
   Q_OBJECT

public:
   explicit ChrBankItemsTabWidget(QWidget *parent = 0);
   ~ChrBankItemsTabWidget();

   void setItems(QList<IChrRomBankItem*> items);
   QModelIndex currentIndex() const;
   void setCurrentIndex(QModelIndex index);

signals:
   void tileSelected(QModelIndex index);
   void snapToTab(QString item);

private:
   Ui::ChrBankItemsTabWidget *ui;

private slots:
   void tileList_clicked(QModelIndex index);
   void tileList_doubleClicked(QModelIndex index);
   void tileList_activated(QModelIndex index);
};

#endif // CHRBANKITEMSTABWIDGET_H
