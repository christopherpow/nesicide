#ifndef FILEPROPERTIESDIALOG_H
#define FILEPROPERTIESDIALOG_H

#include <QDialog>
#include <QUuid>

#include "cprojectbase.h"

namespace Ui {
class FilePropertiesDialog;
}

class FilePropertiesDialog : public QDialog
{
   Q_OBJECT

public:
   explicit FilePropertiesDialog(CProjectBase *base, QWidget *parent = nullptr);
   ~FilePropertiesDialog();

protected:
   CProjectBase *_base;

private slots:
   void on_buttonBox_accepted();

   void on_buttonBox_rejected();

private:
   Ui::FilePropertiesDialog *ui;
};

#endif // FILEPROPERTIESDIALOG_H
