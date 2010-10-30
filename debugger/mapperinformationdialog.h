#ifndef MAPPERINFORMATIONDIALOG_H
#define MAPPERINFORMATIONDIALOG_H

#include <QDialog>

namespace Ui {
    class MapperInformationDialog;
}

class MapperInformationDialog : public QDialog {
    Q_OBJECT
public:
    MapperInformationDialog(QWidget *parent = 0);
    ~MapperInformationDialog();

protected:
    void showEvent(QShowEvent *e);
    void changeEvent(QEvent *e);

public slots:
    void updateInformation();
    void cartridgeLoaded();

signals:
    void showMe();

private:
    Ui::MapperInformationDialog *ui;
};

#endif // MAPPERINFORMATIONDIALOG_H
