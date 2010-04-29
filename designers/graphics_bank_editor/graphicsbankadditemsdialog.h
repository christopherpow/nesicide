#ifndef GRAPHICSBANKADDITEMSDIALOG_H
#define GRAPHICSBANKADDITEMSDIALOG_H

#include <QDialog>
#include "cchrromitemlistdisplaymodel.h"
#include "iprojecttreeviewitem.h"
#include "ichrrombankitem.h"

namespace Ui {
    class GraphicsBankAddItemsDialog;
}

class GraphicsBankAddItemsDialog : public QDialog {
    Q_OBJECT
public:
    GraphicsBankAddItemsDialog(QWidget *parent = 0);
    ~GraphicsBankAddItemsDialog();
    IChrRomBankItem* getSelectedBankItem();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::GraphicsBankAddItemsDialog *ui;
    CChrRomItemListDisplayModel* model;

private slots:
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();
    void enumChildren(IProjectTreeViewItem* item);
};

#endif // GRAPHICSBANKADDITEMSDIALOG_H
