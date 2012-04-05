#ifndef BREAKPOINTDIALOG_H
#define BREAKPOINTDIALOG_H

#include <QDialog>

#include "cregisterdata.h"
#include "cbreakpointinfo.h"

namespace Ui
{
class BreakpointDialog;
}

class BreakpointDialog : public QDialog
{
   Q_OBJECT
public:
   BreakpointDialog(CBreakpointInfo* pBreakpoints,int bp = -1, QWidget* parent = 0);
   virtual ~BreakpointDialog();
   BreakpointInfo* getBreakpoint() { return &m_breakpoint; }

protected:
   void changeEvent(QEvent* e);

private:
   Ui::BreakpointDialog* ui;
   CBreakpointInfo* m_pBreakpoints;
   CRegisterData* m_pRegister;
   CBitfieldData* m_pBitfield;
   CBreakpointEventInfo* m_pEvent;
   void DisplayBreakpoint ( int idx );
   void DisplayResolutions(BreakpointInfo* pBreakpoint);
   BreakpointInfo m_breakpoint;

private slots:
   void on_resolve_clicked();
   void on_addBreakpoint_clicked();
   void on_cancel_clicked();
   void on_addr1_textChanged(QString text);
   void on_event_currentIndexChanged(int index);
   void on_bitfield_currentIndexChanged(int index);
   void on_reg_currentIndexChanged(int index);
   void on_type_currentIndexChanged(int index);
};

#endif // BREAKPOINTDIALOG_H
