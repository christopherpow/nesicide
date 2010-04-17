#ifndef CODEBROWSERDIALOG_H
#define CODEBROWSERDIALOG_H

#include <QDialog>
#include "ccodebrowserdisplaymodel.h"
#include "csourcebrowserdisplaymodel.h"

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
    CCodeBrowserDisplayModel *assemblyViewModel;
    CSourceBrowserDisplayModel *sourceViewModel;

private slots:
    void on_displayMode_currentIndexChanged(int index);
    void on_actionRun_to_here_triggered();
    void on_actionBreak_on_CPU_execution_here_triggered();
};

#endif // CODEBROWSERDIALOG_H
