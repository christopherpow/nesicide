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
    void updateDisassembly(bool show);
    void breakpointHit();

signals:
    void showMe();
    void breakpointsChanged();

private:
    Ui::CodeBrowserDialog *ui;
    CCodeBrowserDisplayModel *assemblyViewModel;
    CSourceBrowserDisplayModel *sourceViewModel;
    int m_breakpointIndex;

private slots:
    void on_actionEnable_breakpoint_triggered();
    void on_actionRemove_breakpoint_triggered();
    void on_actionDisable_breakpoint_triggered();
    void on_tableView_doubleClicked(QModelIndex index);
    void on_displayMode_currentIndexChanged(int index);
    void on_actionRun_to_here_triggered();
    void on_actionBreak_on_CPU_execution_here_triggered();
    void on_actionStart_marker_here_triggered();
    void on_actionEnd_marker_here_triggered();
    void on_actionClear_marker_triggered();
};

#endif // CODEBROWSERDIALOG_H
