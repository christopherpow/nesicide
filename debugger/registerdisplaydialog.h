#ifndef REGISTERDISPLAYDIALOG_H
#define REGISTERDISPLAYDIALOG_H

#include <QDialog>
#include "cdebuggermemorydisplaymodel.h"
#include "cdebuggerregisterdisplaymodel.h"

#include "cnesicidecommon.h"

namespace Ui {
    class RegisterDisplayDialog;
}

class RegisterDisplayDialog : public QDialog {
    Q_OBJECT
public:
    RegisterDisplayDialog(QWidget *parent = 0, eMemoryType display = eMemory_IOregs);
    ~RegisterDisplayDialog();

protected:
    void changeEvent(QEvent *e);

public slots:
   void updateMemory();

private:
    Ui::RegisterDisplayDialog *ui;
    CDebuggerMemoryDisplayModel *binaryModel;
    CDebuggerRegisterDisplayModel *bitfieldModel;

private slots:
    void on_binaryView_doubleClicked(QModelIndex index);
    void on_binaryView_clicked(QModelIndex index);
};

#endif // REGISTERDISPLAYDIALOG_H
