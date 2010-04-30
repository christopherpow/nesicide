#ifndef PPUINFORMATIONDIALOG_H
#define PPUINFORMATIONDIALOG_H

#include <QDialog>

namespace Ui {
    class PPUInformationDialog;
}

class PPUInformationDialog : public QDialog {
    Q_OBJECT
public:
    PPUInformationDialog(QWidget *parent = 0);
    ~PPUInformationDialog();

protected:
    void showEvent(QShowEvent *e);
    void changeEvent(QEvent *e);

public slots:
    void updateInformation();

signals:
    void showMe();

private:
    Ui::PPUInformationDialog *ui;
};

#endif // PPUINFORMATIONDIALOG_H
