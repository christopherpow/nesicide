#ifndef BREAKPOINTDIALOG_H
#define BREAKPOINTDIALOG_H

#include <QDialog>

#include "cregisterdata.h"

namespace Ui {
   class BreakpointDialog;
}

class BreakpointDialog : public QDialog {
   Q_OBJECT
public:
   BreakpointDialog(QWidget *parent = 0);
   ~BreakpointDialog();

protected:
   void changeEvent(QEvent *e);

private:
   Ui::BreakpointDialog *ui;
   CRegisterData* m_pRegister;

private slots:
    void on_register_2_currentIndexChanged(int index);
    void on_pushButton_clicked();
    void on_type_currentIndexChanged(int index);
};

#endif // BREAKPOINTDIALOG_H
