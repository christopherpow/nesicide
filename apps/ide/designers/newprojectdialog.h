#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

#include <QDialog>
#include <QListWidgetItem>

namespace Ui
{
class NewProjectDialog;
}

class NewProjectDialog : public QDialog
{
   Q_OBJECT
public:
   NewProjectDialog(QString windowTitle = "New",QString defName = "", QString defPath = "", bool addOnsOnly = false, QWidget* parent = 0);
   virtual ~NewProjectDialog();
   QString getName();
   QString getPath();
   QString getTarget();
   QString getTargetShort();
   QString getTemplate();
   int getTemplateIndex();
   QStringList getAddOns();

protected:
   void changeEvent(QEvent* e);
   bool checkValidity();
   bool eventFilter(QObject *obj, QEvent *event);

private:
   Ui::NewProjectDialog* ui;
   void setupProject();

private slots:
    void on_target_currentIndexChanged(QString target);
    void on_buttonBox_accepted();
    void on_name_textChanged(QString text);
    void on_path_textChanged(QString text);
    void on_pathBrowse_clicked();
    void on_projectProperties_clicked();
    void on_addOnsList_itemClicked(QListWidgetItem *item);
    void on_addOnsList_itemEntered(QListWidgetItem *item);
};

#endif // NEWPROJECTDIALOG_H
