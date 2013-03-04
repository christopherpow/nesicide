#ifndef INSTRUMENTEDITDIALOG_H
#define INSTRUMENTEDITDIALOG_H

#include <QDialog>
#include "FamiTrackerDoc.h"

namespace Ui {
class CInstrumentEditDlg;
}

class CInstrumentEditDlg : public QDialog
{
   Q_OBJECT
   
public:
   explicit CInstrumentEditDlg(CFamiTrackerDoc* pDoc,QDialog *parent = 0);
   ~CInstrumentEditDlg();
   
private:
   Ui::CInstrumentEditDlg *ui;
};

#endif // INSTRUMENTEDITDIALOG_H
