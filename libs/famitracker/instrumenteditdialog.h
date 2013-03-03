#ifndef INSTRUMENTEDITDIALOG_H
#define INSTRUMENTEDITDIALOG_H

#include <QDialog>
#include "FamiTrackerDoc.h"

namespace Ui {
class CInstrumentEditDialog;
}

class CInstrumentEditDialog : public QDialog
{
   Q_OBJECT
   
public:
   explicit CInstrumentEditDialog(CFamiTrackerDoc* pDoc,QDialog *parent = 0);
   ~CInstrumentEditDialog();
   
private:
   Ui::CInstrumentEditDialog *ui;
};

#endif // INSTRUMENTEDITDIALOG_H
