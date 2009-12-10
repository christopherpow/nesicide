#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

#include <QDialog>

namespace Ui {
    class NewProjectDialog;
}

class NewProjectDialog : public QDialog {
    Q_OBJECT
public:
    NewProjectDialog(QWidget *parent = 0);
    ~NewProjectDialog();
    QString getProjectTitle();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::NewProjectDialog *ui;
};

#endif // NEWPROJECTDIALOG_H
