#ifndef NEWFILEDIALOG_H
#define NEWFILEDIALOG_H

#include <QDialog>

namespace Ui
{
class NewFileDialog;
}

class NewFileDialog : public QDialog
{
   Q_OBJECT
public:
   NewFileDialog(QString windowTitle = "New",QString defName = "", QString defPath = "", QWidget* parent = 0);
   virtual ~NewFileDialog();
   QString getName();
   QString getPath();

protected:
   void changeEvent(QEvent* e);
   bool checkValidity();

private:
   Ui::NewFileDialog* ui;

private slots:
    void on_buttonBox_accepted();
    void on_name_textChanged(QString text);
    void on_path_textChanged(QString text);
    void on_pathBrowse_clicked();
};

#endif // NEWFILEDIALOG_H
