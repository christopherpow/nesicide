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
   NewProjectDialog(QWidget* parent = 0,QString windowTitle = "New",QString defName = "", QString defPath = "", bool showTemplate = false);
   virtual ~NewProjectDialog();
   QString getName();
   QString getPath();
   QString getTarget();
   QString getTemplate();
   int getTemplateIndex();

protected:
   void changeEvent(QEvent* e);
   bool checkValidity();

private:
   Ui::NewProjectDialog* ui;

private slots:
    void on_target_currentIndexChanged(QString target);
    void on_buttonBox_accepted();
    void on_name_textChanged(QString text);
    void on_path_textChanged(QString text);
    void on_pathBrowse_clicked();
};

#endif // NEWPROJECTDIALOG_H
