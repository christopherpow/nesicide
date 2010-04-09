#ifndef BREAKPOINTDIALOG_H
#define BREAKPOINTDIALOG_H

#include <QDialog>

#include "cregisterdata.h"
#include "cbreakpointinfo.h"
#include "cbreakpointdisplaymodel.h"
#include "cnes.h"

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
   void showEvent(QShowEvent *e);

private:
   Ui::BreakpointDialog *ui;
   CRegisterData* m_pRegister;
   CBitfieldData* m_pBitfield;
   CBreakpointEventInfo* m_pEvent;
   CBreakpointDisplayModel* model;
   void DisplayBreakpoint ( int idx );

signals:
   void showMe();

private slots:
    void on_endisButton_clicked();
    void on_modifyButton_clicked();
    void on_addr1_textChanged(QString );
    void on_removeButton_clicked();
    void on_event_currentIndexChanged(int index);
    void on_tableView_pressed(QModelIndex index);
    void on_tableView_entered(QModelIndex index);
    void on_tableView_doubleClicked(QModelIndex index);
    void on_tableView_clicked(QModelIndex index);
    void on_tableView_activated(QModelIndex index);
    void on_bitfield_currentIndexChanged(int index);
    void on_reg_currentIndexChanged(int index);
    void on_addButton_clicked();
    void on_type_currentIndexChanged(int index);
    void updateData();
};

#endif // BREAKPOINTDIALOG_H
