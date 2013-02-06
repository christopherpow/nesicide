#ifndef FAMITRACKERMODULEIMPORTDIALOG_H
#define FAMITRACKERMODULEIMPORTDIALOG_H

#include <QDialog>

#include "FamiTrackerDoc.h"

#include <QStringListModel>

namespace Ui {
class FamiTrackerModuleImportDialog;
}

class FamiTrackerModuleImportDialog : public QDialog
{
   Q_OBJECT
   
public:
   explicit FamiTrackerModuleImportDialog(CFamiTrackerDoc* pDoc,QWidget *parent = 0);
   ~FamiTrackerModuleImportDialog();
   
   bool loadFile(QString path);
   
private slots:
   void on_buttonBox_accepted();
   
private:
   Ui::FamiTrackerModuleImportDialog *ui;
   CFamiTrackerDoc* m_pDocument;
   CFamiTrackerDoc* m_pImportedDoc;
   QStringListModel* tracksModel;
   int m_iInstrumentTable[MAX_INSTRUMENTS];   
   
   bool importInstruments();
   bool importTracks();
};

#endif // FAMITRACKERMODULEIMPORTDIALOG_H
