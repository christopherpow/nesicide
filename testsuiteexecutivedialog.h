#ifndef TESTSUITEEXECUTIVEDIALOG_H
#define TESTSUITEEXECUTIVEDIALOG_H

#include <QDialog>

class MainWindow;

namespace Ui {
    class TestSuiteExecutiveDialog;
}

class TestSuiteExecutiveDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TestSuiteExecutiveDialog(QWidget *parent = 0);
    virtual ~TestSuiteExecutiveDialog();

private:
    Ui::TestSuiteExecutiveDialog *ui;
    bool aborted;
    bool running;
    void loadTestSuite(QString testSuiteFileName);
    void executeTests(int start,int end);

signals:
    void openROM(QString romFile);
    void startEmulation();
    void pauseEmulationAfter(int32_t frames);

private slots:
    void on_save_clicked();
    void on_clear_clicked();
    void on_load_clicked();
    void on_execute_clicked();
    void on_executeSelection_clicked();
    void on_abort_clicked();
    void updateProgress();
    void emulatorPausedAfter();
};

#endif // TESTSUITEEXECUTIVEDIALOG_H
