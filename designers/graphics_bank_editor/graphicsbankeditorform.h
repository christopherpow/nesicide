#ifndef GRAPHICSBANKEDITORFORM_H
#define GRAPHICSBANKEDITORFORM_H

#include <QWidget>
#include <QList>
#include "graphicsbankadditemsdialog.h"
#include "cchrromitemlistdisplaymodel.h"
#include "ichrrombankitem.h"

namespace Ui {
    class GraphicsBankEditorForm;
}

class GraphicsBankEditorForm : public QWidget {
    Q_OBJECT
public:
    GraphicsBankEditorForm(QWidget *parent = 0);
    ~GraphicsBankEditorForm();
    QList<IChrRomBankItem*> chrRomBankItems;

protected:
    void changeEvent(QEvent *e);

private:
    Ui::GraphicsBankEditorForm *ui;
    CChrRomItemListDisplayModel *model;

private slots:
    void on_AddItemPushButton_clicked();
    void on_removeItemPushButton_clicked();
};

#endif // GRAPHICSBANKEDITORFORM_H
