#ifndef APUINFORMATIONDIALOG_H
#define APUINFORMATIONDIALOG_H

#include <QDialog>

namespace Ui {
    class APUInformationDialog;
}

class APUInformationDialog : public QDialog {
    Q_OBJECT
public:
    APUInformationDialog(QWidget *parent = 0);
    ~APUInformationDialog();

protected:
    void showEvent(QShowEvent *e);
    void changeEvent(QEvent *e);

public slots:
    void updateInformation();

signals:
    void showMe();

private:
    Ui::APUInformationDialog *ui;
};

#endif // APUINFORMATIONDIALOG_H
