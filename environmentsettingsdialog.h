#ifndef ENVIRONMENTSETTINGSDIALOG_H
#define ENVIRONMENTSETTINGSDIALOG_H

#include <QDialog>
#include <QColorDialog>

namespace Ui {
    class EnvironmentSettingsDialog;
}

class EnvironmentSettingsDialog : public QDialog {
    Q_OBJECT
public:
    EnvironmentSettingsDialog(QWidget *parent = 0);
    ~EnvironmentSettingsDialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::EnvironmentSettingsDialog *ui;
    QColor getIdealTextColor(const QColor& rBackgroundColor) const;

private slots:
    void on_CodeStringsButton_clicked();
    void on_CodeNumbersButton_clicked();
    void on_CodePreprocessorButton_clicked();
    void on_CodeCommentsButton_clicked();
    void on_CodeInstructionsButton_clicked();
    void on_CodeDefaultButton_clicked();
    void on_CodeBackgroundButton_clicked();
};

#endif // ENVIRONMENTSETTINGSDIALOG_H
