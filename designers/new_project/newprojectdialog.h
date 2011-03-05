#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

#include <QDialog>

namespace Ui
{
class NewProjectDialog;
}

class NewProjectDialog : public QDialog
{
   Q_OBJECT
public:
   NewProjectDialog(QWidget* parent = 0);
   ~NewProjectDialog();
   QString getProjectTitle();
   QString getProjectBasePath();

protected:
   void changeEvent(QEvent* e);

private:
   Ui::NewProjectDialog* ui;

private slots:
    void on_projectBasePathBrowse_clicked();
};

#endif // NEWPROJECTDIALOG_H
