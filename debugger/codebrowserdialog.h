#ifndef CODEBROWSERDIALOG_H
#define CODEBROWSERDIALOG_H

#include <QDialog>
#include "ccodebrowserdisplaymodel.h"

namespace Ui {
    class CodeBrowserDialog;
}

class CodeBrowserDialog : public QDialog {
    Q_OBJECT
public:
    CodeBrowserDialog(QWidget *parent = 0);
    ~CodeBrowserDialog();

protected:
    void showEvent(QShowEvent *e);
    void contextMenuEvent(QContextMenuEvent *e);
    void changeEvent(QEvent *e);

public slots:
    void updateBrowser();
    void updateDisassembly();

signals:
    void showMe();

private:
    Ui::CodeBrowserDialog *ui;
    CCodeBrowserDisplayModel *tableViewModel;
};

#endif // CODEBROWSERDIALOG_H
