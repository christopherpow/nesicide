#ifndef PRGROMDISPLAYDIALOG_H
#define PRGROMDISPLAYDIALOG_H

#include <QDialog>

namespace Ui {
    class PRGROMDisplayDialog;
}

class PRGROMDisplayDialog : public QDialog {
    Q_OBJECT
public:
    PRGROMDisplayDialog(QWidget *parent = 0);
    ~PRGROMDisplayDialog();
    void setRomData(QString data);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::PRGROMDisplayDialog *ui;
};

#endif // PRGROMDISPLAYDIALOG_H
