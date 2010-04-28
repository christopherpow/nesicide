#ifndef GRAPHICSBANKEDITORFORM_H
#define GRAPHICSBANKEDITORFORM_H

#include <QWidget>
#include "graphicsbankadditemsdialog.h"

namespace Ui {
    class GraphicsBankEditorForm;
}

class GraphicsBankEditorForm : public QWidget {
    Q_OBJECT
public:
    GraphicsBankEditorForm(QWidget *parent = 0);
    ~GraphicsBankEditorForm();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::GraphicsBankEditorForm *ui;

private slots:
    void on_AddItemPushButton_clicked();
    void on_removeItemPushButton_clicked();
};

#endif // GRAPHICSBANKEDITORFORM_H
