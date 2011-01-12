#ifndef OUTPUTDIALOG_H
#define OUTPUTDIALOG_H

#include <QDialog>

namespace Ui
{
class OutputDialog;
}

class OutputDialog : public QDialog
{
   Q_OBJECT

public:
   enum
   {
      Output_General = 0,
      Output_Build,
      Output_Debug
   };
   explicit OutputDialog(QWidget* parent = 0);
   ~OutputDialog();

   void setCurrentOutputTab ( int tab );
   void clearAllTabs();
   void clearTab(int tab);

protected:
   virtual void contextMenuEvent ( QContextMenuEvent* event );

private slots:
   void updateData();

private:
   Ui::OutputDialog* ui;
};

#endif // OUTPUTDIALOG_H
